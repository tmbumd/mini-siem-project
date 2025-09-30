FROM python:3.11-slim
WORKDIR /replay
COPY tools/replay.py .
ENTRYPOINT ["python","/replay/replay.py"]
