#include "common.h"
#include "syslog_parse.h"

static void iso_now(char *buf, size_t blen){
    time_t t=time(NULL); struct tm tm; gmtime_r(&t,&tm);
    strftime(buf, blen, "%Y-%m-%dT%H:%M:%SZ", &tm);
}

int parse_syslog_line(const char *in, syslog_evt_t *out){
    memset(out, 0, sizeof(*out));
    out->pri = -1; out->is_5424 = 0; iso_now(out->ts, sizeof(out->ts));

    const char *p=in; size_t len=strlen(in);
    if(len<3) return -1;

    if(p[0]=='<' ){ char *end = strchr(p,'>'); if(end){ out->pri=atoi(p+1); p=end+1; } }

    if(isdigit((unsigned char)p[0]) && isdigit((unsigned char)p[1]) && isdigit((unsigned char)p[2]) && isdigit((unsigned char)p[3])){
        out->is_5424=1;
        const char *sp = strchr(p,' ');
        if(sp && (sp-p)<(int)sizeof(out->ts)) { strncpy(out->ts, p, sp-p); out->ts[sp-p]='\0'; p=sp+1; }
        sp = strchr(p,' ');
        if(sp && (sp-p)<(int)sizeof(out->host)) { strncpy(out->host, p, sp-p); out->host[sp-p]='\0'; p=sp+1; }
        sp = strchr(p,' ');
        const char *col = strchr(p, ':');
        const char *stop = sp && col ? (sp<col?sp:col) : (sp?sp:(col?col:NULL));
        if(stop && (stop-p)<(int)sizeof(out->app)) { strncpy(out->app, p, stop-p); out->app[stop-p]='\0'; if(stop==col) p=col+1; else p=sp+1; }
        snprintf(out->msg, sizeof(out->msg), "%s", p);
    } else {
        const char *sp = strchr(p,' '); if(!sp) sp=p;
        sp = strchr(sp+1,' '); if(!sp) sp=p;
        sp = strchr(sp+1,' '); if(!sp) sp=p;
        const char *hstart = sp; while(*hstart==' ') hstart++;
        const char *hend = strchr(hstart,' ');
        if(hend && (hend-hstart)<(int)sizeof(out->host)) { strncpy(out->host, hstart, hend-hstart); out->host[hend-hstart]='\0'; p=hend+1; }
        const char *col = strchr(p, ':');
        if(col){ size_t alen = (size_t)(col - p); if(alen >= sizeof(out->app)) alen=sizeof(out->app)-1; strncpy(out->app,p,alen); out->app[alen]='\0'; p=col+1; }
        snprintf(out->msg, sizeof(out->msg), "%s", p);
    }
    trim(out->msg);
    return 0;
}
