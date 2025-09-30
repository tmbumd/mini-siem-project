FROM debian:stable-slim AS build
RUN apt-get update && apt-get install -y build-essential && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY src src
COPY Makefile .
RUN make && mkdir -p /opt/mini-siem/bin && cp bin/siem_collector /opt/mini-siem/bin/

FROM debian:stable-slim
WORKDIR /opt/mini-siem
RUN useradd -m siem && mkdir -p data out && chown -R siem:siem /opt/mini-siem
COPY --from=build /opt/mini-siem/bin/siem_collector /opt/mini-siem/bin/siem_collector
USER siem
EXPOSE 5514/udp 5514/tcp
ENTRYPOINT ["/opt/mini-siem/bin/siem_collector","-p","5514","-o","data/events.jsonl"]
