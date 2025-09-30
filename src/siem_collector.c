#include "common.h"
#include "syslog_parse.h"

static void json_escape(const char *in, char *out, size_t outsz){
    size_t j=0; for(size_t i=0; in[i] && j+2<outsz; ++i){
        unsigned char c=in[i];
        if(c=='"'||c=='\\') { out[j++]='\\'; out[j++]=c; }
        else if(c=='\n'){ out[j++]='\\'; out[j++]='n'; }
        else { out[j++]=c; }
    }
    out[j]='\0';
}

static int write_jsonl(FILE *f, const syslog_evt_t *e){
    char msgesc[8192]; json_escape(e->msg, msgesc, sizeof(msgesc));
    fprintf(f, "{\"ts\":\"%s\",\"pri\":%d,\"host\":\"%s\",\"app\":\"%s\",\"msg\":\"%s\"}\n",
            e->ts, e->pri, e->host, e->app, msgesc);
    fflush(f);
    return 0;
}

static int make_udp_socket(int port){
    int s = socket(AF_INET, SOCK_DGRAM, 0); if(s<0) die("socket udp");
    int yes=1; setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes));
    struct sockaddr_in addr={0}; addr.sin_family=AF_INET; addr.sin_addr.s_addr=INADDR_ANY; addr.sin_port=htons(port);
    if(bind(s,(struct sockaddr*)&addr,sizeof(addr))<0) die("bind udp");
    return s;
}

static int make_tcp_socket(int port){
    int s = socket(AF_INET, SOCK_STREAM, 0); if(s<0) die("socket tcp");
    int yes=1; setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes));
    struct sockaddr_in addr={0}; addr.sin_family=AF_INET; addr.sin_addr.s_addr=INADDR_ANY; addr.sin_port=htons(port);
    if(bind(s,(struct sockaddr*)&addr,sizeof(addr))<0) die("bind tcp");
    if(listen(s, 64)<0) die("listen");
    return s;
}

int main(int argc, char **argv){
    int port=5514; const char *outfile="data/events.jsonl";
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"-p")&&i+1<argc) port=atoi(argv[++i]);
        else if(!strcmp(argv[i],"-o")&&i+1<argc) outfile=argv[++i];
        else { fprintf(stderr,"Usage: %s [-p port] [-o outfile]\n", argv[0]); return 1; }
    }
    mkdir("data", 0755);
    FILE *f = fopen(outfile, "a"); if(!f) die("open outfile");

    int udp = make_udp_socket(port);
    int tcp = make_tcp_socket(port);

    fd_set rfds; int maxfd = udp>tcp?udp:tcp; char buf[BUF_SZ];
    fprintf(stderr, "[collector] listening UDP/TCP on %d -> %s\n", port, outfile);

    while(1){
        FD_ZERO(&rfds); FD_SET(udp,&rfds); FD_SET(tcp,&rfds);
        struct timeval tv={1,0}; int rv = select(maxfd+1,&rfds,NULL,NULL,&tv);
        if(rv<0){ if(errno==EINTR) continue; die("select"); }
        if(FD_ISSET(udp,&rfds)){
            struct sockaddr_in src; socklen_t slen=sizeof(src);
            ssize_t n = recvfrom(udp, buf, sizeof(buf)-1, 0, (struct sockaddr*)&src,&slen);
            if(n>0){ buf[n]='\0'; syslog_evt_t e; parse_syslog_line(buf,&e);
                if(!e.host[0]) snprintf(e.host,sizeof(e.host),"%s", inet_ntoa(src.sin_addr));
                write_jsonl(f,&e);
            }
        }
        if(FD_ISSET(tcp,&rfds)){
            int cs = accept(tcp,NULL,NULL); if(cs<0) continue;
            ssize_t n; while((n=recv(cs,buf,sizeof(buf)-1,0))>0){
                buf[n]='\0'; char *save=NULL; char *line=strtok_r(buf,"\n",&save);
                while(line){ syslog_evt_t e; parse_syslog_line(line,&e); write_jsonl(f,&e); line=strtok_r(NULL,"\n",&save); }
            }
            close(cs);
        }
    }
}
