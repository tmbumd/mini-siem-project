#ifndef SYSLOG_PARSE_H
#define SYSLOG_PARSE_H
#include <time.h>

typedef struct {
    int pri;
    char ts[64];
    char host[128];
    char app[64];
    char msg[4096];
    int is_5424;
} syslog_evt_t;

int parse_syslog_line(const char *in, syslog_evt_t *out);

#endif
