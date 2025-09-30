# Mini-SIEM — One-Pager (Hiring Friendly)

What it is. A compact, open-source SIEM I built to prove end-to-end detection engineering: C syslog collector + Python rule engine (YAML) + Docker demo + Grafana.

Why it matters. Small teams drown in noisy logs or can’t afford heavy platforms. This shows I can design, ship, and harden practical blue-team tooling rapidly.

Key features
- RFC3164/5424 ingestion (UDP/TCP) -> JSONL + SQLite
- Sliding-window correlation + regex/contains rules (YAML)
- 1-command Docker demo, Grafana dashboard
- Replay pack: SSH brute force, sudo escalation, kernel panic
- CI build/test; benchmarks script

How to run
```bash
docker compose up -d --build && docker compose run --rm replay
```

Results (example numbers — replace with your run)
- ~50k EPS UDP ingest on laptop; alert latency <300ms
- 70% FPs reduction after tuning thresholds/regex
- Containerized, reproducible, 90s end-to-end demo

Links
- GitHub: (add your repo link)
- 90-second demo video: (add link)
