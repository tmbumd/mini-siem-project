CC=gcc
CFLAGS=-O2 -Wall -Wextra -Isrc
BIN=bin
SRC=src

all: $(BIN)/siem_collector $(BIN)/siem_agent

$(BIN)/siem_collector: $(SRC)/siem_collector.c $(SRC)/syslog_parse.c $(SRC)/common.h $(SRC)/syslog_parse.h
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) -o $@ $(SRC)/siem_collector.c $(SRC)/syslog_parse.c

$(BIN)/siem_agent: $(SRC)/siem_agent.c $(SRC)/common.h
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) -o $@ $(SRC)/siem_agent.c

clean:
	rm -rf $(BIN) data out events.db
