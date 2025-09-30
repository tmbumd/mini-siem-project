# Mini-SIEM — C Collector + Python Rule Engine (Docker Demo)

A lean, hackable SIEM you can run locally or on a small VPS. Ingests syslog, normalizes to JSON, stores to newline-delimited JSON and SQLite, and evaluates YAML detection rules. Includes a push-button Docker demo, Grafana with SQLite datasource plugin, replayed attacks, benchmarks, and CI.

## One-command demo
```bash
docker compose up -d --build
# in a second terminal, start the replay
docker compose run --rm replay
# open Grafana at http://localhost:3000 (user/pass: admin/admin by default)
```
You should see events flowing and basic tables showing Top Hosts and Recent Events.

## Services
- collector: C syslog listener (UDP/TCP 5514) writing data/events.jsonl
- indexer: Python rules engine tailing events -> SQLite data/events.db and out/alerts.log
- grafana: Grafana with SQLite datasource plugin provisioned; dashboard auto-loads
- replay: sends sample logs (SSH brute force, sudo, kernel oops)

## Local (non-Docker) quick start
```bash
make
./bin/siem_collector -p 5514 -o data/events.jsonl &
python3 indexer.py --events data/events.jsonl --rules rules.yml --alerts out/alerts.log &
python3 tools/replay.py --paths replays/ssh_bruteforce.log,replays/sudo.log,replays/kernel_oops.log
python3 cli.py top --field host --n 10
```

## Repo layout
```
src/            C sources (collector, parser, agent)
docker/         Dockerfiles
provisioning/   Grafana provisioning (datasource + dashboard)
replays/        Sample attack logs
tools/          Replay + benchmarks
.github/workflows/ci.yml  CI build + lightweight tests
```

## Security/Hygiene
- Bind to private networks or firewall public ports.
- Prefer TCP+TLS (mTLS) for production (roadmap).
- Input sanitization + JSON escaping (present).
- Add rate-limits and backpressure before internet exposure.

## Roadmap
- mTLS for TCP syslog and agent
- ClickHouse sink + dashboards for multi-million row scale
- Windows ingestion via NxLog/Winlogbeat -> syslog
- Distinct-threshold and join/chain rules
- REST alert webhooks (Slack/Discord/Teams), suppression windows
