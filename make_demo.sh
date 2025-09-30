#!/usr/bin/env bash
set -euo pipefail
docker compose up -d --build
sleep 4
docker compose run --rm replay
echo "Open Grafana at http://localhost:3000 (admin/admin)."
