#ifndef COMMON_H
#define COMMON_H
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>

#define BUF_SZ 8192

static inline void die(const char *msg){perror(msg); exit(EXIT_FAILURE);}
static inline void trim(char *s){
    size_t n = strlen(s); while(n && (s[n-1]=='\n'||s[n-1]=='\r'||isspace((unsigned char)s[n-1]))) s[--n]='\0';
}

#endif
