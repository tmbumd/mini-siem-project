#!/usr/bin/env python3
import argparse, sqlite3
ap = argparse.ArgumentParser()
sub = ap.add_subparsers(dest='cmd')
p=sub.add_parser('top'); p.add_argument('--field', required=True); p.add_argument('--n', type=int, default=10)
args=ap.parse_args()
conn = sqlite3.connect('data/events.db')
cur = conn.cursor()
if args.cmd=='top':
    field = args.field
    cur.execute(f"SELECT {field}, COUNT(*) FROM events GROUP BY {field} ORDER BY COUNT(*) DESC LIMIT ?", (args.n,))
    for row in cur.fetchall():
        print(f"{row[0]:30} {row[1]}")
