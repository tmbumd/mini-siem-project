#include "common.h"

static int make_tcp_client(const char *host, int port){
    int s = socket(AF_INET, SOCK_STREAM, 0); if(s<0) die("socket");
    struct sockaddr_in addr={0}; addr.sin_family=AF_INET; addr.sin_port=htons(port);
    if(inet_pton(AF_INET, host, &addr.sin_addr)<=0) die("inet_pton (use IPv4)");
    if(connect(s,(struct sockaddr*)&addr,sizeof(addr))<0) die("connect");
    return s;
}

int main(int argc, char **argv){
    const char *file=NULL, *host="127.0.0.1"; int port=5515;
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"-f")&&i+1<argc) file=argv[++i];
        else if(!strcmp(argv[i],"-c")&&i+1<argc) host=argv[++i];
        else if(!strcmp(argv[i],"-p")&&i+1<argc) port=atoi(argv[++i]);
        else { fprintf(stderr,"Usage: %s -f /path/to/log [-c host] [-p port]\n", argv[0]); return 1; }
    }
    if(!file) { fprintf(stderr,"log file required\n"); return 1; }

    int s = make_tcp_client(host, port);
    FILE *fp = fopen(file, "r"); if(!fp) die("open log");
    fseek(fp, 0, SEEK_END);
    char buf[BUF_SZ];
    fprintf(stderr, "[agent] shipping %s -> %s:%d\n", file, host, port);

    while(1){
        char *line = fgets(buf, sizeof(buf), fp);
        if(!line){ usleep(200000); clearerr(fp); continue; }
        size_t n = strlen(line);
        if(send(s, line, n, 0)<0) die("send");
    }
}
