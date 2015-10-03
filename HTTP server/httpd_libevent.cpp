#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <event.h>
#include <evhttp.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>

void signal_handler(int sig)
{
    switch(sig) {
        case SIGTERM:
        case SIGHUP:
        case SIGQUIT:
        case SIGINT:
            event_loopbreak();
        break;
    }
}
char output[2048];
void httpd_handler(struct evhttp_request *req,void *arg)
{
    const char *uri;
    uri = evhttp_request_uri(req);

    printf("visitor connencted %s\n",uri);
    //HTTP header
    evhttp_add_header(req->output_headers,"Server","cmlin's httpd server");
    evhttp_add_header(req->output_headers,"Content-Type","text/html;charset=UTF-8");
    evhttp_add_header(req->output_headers,"Connection","close");



    struct evbuffer *buf;
    buf = evbuffer_new();
    //evbuffer_add_printf(buf,"%s",output);
    if(strcmp(uri,"/") == 0) {
        evbuffer_add(buf,output,2048);
    }else if(strstr(uri,".jpg")) {
        char path[80] = "web";
        strcat(path,uri);
        int fd = open(path,O_RDONLY);
        struct stat st;
        fstat(fd,&st);
        evbuffer_add_file(buf,fd,0,st.st_size);
    }
    /*
    int fd = open("web/1.jpg",O_RDONLY);
    struct stat st;
    fstat(fd,&st);

    evbuffer_add_file(buf,fd,0,st.st_size);
    */
    evhttp_send_reply(req,HTTP_OK,"OK",buf);
    evbuffer_free(buf);


}
int main()
{
    signal(SIGHUP,signal_handler);
    signal(SIGTERM,signal_handler);
    signal(SIGINT,signal_handler);
    signal(SIGQUIT,signal_handler);

    int httpd_port = 8080;
    int httpd_timeout = 120;

    FILE* f = fopen("web/index.html","r");
    fread(output,2048,1,f);
    fclose(f);


    event_init();

    struct evhttp *httpd;
    httpd = evhttp_start("0.0.0.0",httpd_port);
    evhttp_set_timeout(httpd,httpd_timeout);

    evhttp_set_gencb(httpd,httpd_handler,NULL);

    event_dispatch();

    evhttp_free(httpd);
    return 0;
}
