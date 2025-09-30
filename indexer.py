#!/usr/bin/env python3
import argparse, json, os, re, sys, time, sqlite3, datetime
from collections import deque

class Window:
    def __init__(self, seconds):
        self.buf = deque()
        self.seconds = seconds
    def add(self, ts):
        self.buf.append(ts)
        self.trim(ts)
    def trim(self, now_ts):
        while self.buf and now_ts - self.buf[0] > self.seconds:
            self.buf.popleft()
    def count(self):
        return len(self.buf)

class Rule:
    def __init__(self, r):
        self.id = r.get('id'); self.name = r.get('name', self.id)
        self.when = r.get('when', {})
        self.window = parse_window(r.get('window', '0s'))
        self.threshold = r.get('threshold', 1)
        self.severity = r.get('severity', 'low')
        self.tags = r.get('tags', [])
        self.alert = r.get('alert', 'summary')
        self._regex = re.compile(self.when.get('msg_regex', '.'), re.I) if 'msg_regex' in self.when else None
        self._app_contains = self.when.get('app_contains')
        self._msg_contains = self.when.get('msg_contains')

    def match(self, evt):
        if self._app_contains and self._app_contains not in (evt.get('app') or ''):
            return False
        if self._msg_contains and self._msg_contains not in (evt.get('msg') or ''):
            return False
        if self._regex and not self._regex.search(evt.get('msg') or ''):
            return False
        return True

def parse_window(s):
    if not s: return 0
    m = re.match(r"(\d+)", s); num = int(m.group(1)) if m else 0
    if s.endswith('m'): return num*60
    if s.endswith('h'): return num*3600
    return num

def load_rules(path):
    import yaml
    with open(path,'r') as f:
        raw = yaml.safe_load(f)
    return [Rule(r) for r in raw]

def open_sqlite(path='events.db'):
    conn = sqlite3.connect(path)
    conn.execute("CREATE TABLE IF NOT EXISTS events (ts TEXT, pri INT, host TEXT, app TEXT, msg TEXT)")
    conn.commit(); return conn

def maybe_insert(conn, evt):
    conn.execute("INSERT INTO events(ts, pri, host, app, msg) VALUES(?,?,?,?,?)", (
        evt.get('ts'), evt.get('pri'), evt.get('host'), evt.get('app'), evt.get('msg')
    ))

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--events', required=True)
    ap.add_argument('--rules', required=True)
    ap.add_argument('--alerts', default='out/alerts.log')
    ap.add_argument('--no-sqlite', action='store_true')
    args = ap.parse_args()

    os.makedirs('out', exist_ok=True)
    conn = None if args.no_sqlite else open_sqlite('data/events.db')
    rules = load_rules(args.rules)
    windows = {r.id: Window(r.window) for r in rules}

    with open(args.events, 'r') as f, open(args.alerts,'a') as a:
        f.seek(0, os.SEEK_END)
        while True:
            line = f.readline()
            if not line:
                time.sleep(0.2)
                continue
            try:
                evt = json.loads(line)
            except Exception:
                continue
            if conn: maybe_insert(conn, evt)
            now_ts = int(time.time())
            for r in rules:
                if r.match(evt):
                    w = windows[r.id]
                    w.add(now_ts)
                    if w.count() >= r.threshold:
                        summary = {
                            'time': datetime.datetime.utcnow().isoformat()+"Z",
                            'rule_id': r.id,
                            'name': r.name,
                            'severity': r.severity,
                            'count': w.count(),
                            'last_evt': {'host': evt.get('host'), 'app': evt.get('app'), 'msg': (evt.get('msg') or '')[:200]}
                        }
                        print(json.dumps(summary), file=a)
                        a.flush()
                        w.buf.clear()
            if conn: conn.commit()

if __name__ == '__main__':
    main()
