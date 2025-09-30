FROM python:3.11-slim
WORKDIR /opt/mini-siem
RUN pip install pyyaml
COPY indexer.py cli.py rules.yml ./
RUN mkdir -p out
ENTRYPOINT ["python","/opt/mini-siem/indexer.py","--events","/data/events.jsonl","--rules","/opt/mini-siem/rules.yml","--alerts","/out/alerts.log"]
