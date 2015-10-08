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
char output[8000000] ;

struct evbuffer *buf1;
void httpd_handler(struct evhttp_request *req,void *arg)
{
    const char *uri;
    uri = evhttp_request_uri(req);
    char path[80] = "web";
    //printf("visitor connencted %s\n",uri);
    //HTTP header
    evhttp_add_header(req->output_headers,"Server","cmlin's httpd server");
    evhttp_add_header(req->output_headers,"Connection","close");

	/* only for the experiment
    if(strcmp(uri,"/") == 0) {
        evhttp_add_header(req->output_headers,"Content-Type","text/html");
		evhttp_send_reply(req,HTTP_OK,"OK",buf1);
		return;
	}
	*/

    struct evbuffer *buf;
    buf = evbuffer_new();
    //evbuffer_add_printf(buf,"%s",output);
    if(strcmp(uri,"/") == 0) {
        evhttp_add_header(req->output_headers,"Content-Type","text/html");
		evbuffer_add(buf,output,strlen(output));
    }else if(strstr(uri,".jpg")) {
        strcat(path,uri);
        int fd = open(path,O_RDONLY);
        if(fd < 0) goto err;
        struct stat st;
        fstat(fd,&st);
	evhttp_add_header(req->output_headers,"Content-Type","image/jpeg");
        evbuffer_add_file(buf,fd,0,st.st_size);
	close(fd);
    }else if(strstr(uri,".css")) {
        strcat(path,uri);
        int fd = open(path,O_RDONLY);
        if(fd < 0) goto err;
        struct stat st;
        fstat(fd,&st);
	evhttp_add_header(req->output_headers,"Content-Type","text/css");
        evbuffer_add_file(buf,fd,0,st.st_size);
	close(fd);
    }else if(strstr(uri,".html")) {
	strcat(path,uri);
	int fd = open(path,O_RDONLY);
	if(fd < 0) goto err;
	struct stat st;
	fstat(fd,&st);
	evhttp_add_header(req->output_headers,"Content-Type","text/html");
	evbuffer_add_file(buf,fd,0,st.st_size);
	close(fd);
    }
    /*
    int fd = open("web/1.jpg",O_RDONLY);
    struct stat st;
    fstat(fd,&st);

    evbuffer_add_file(buf,fd,0,st.st_size);
    */
    evhttp_send_reply(req,HTTP_OK,"OK",buf);
    err:
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
    fread(output,8000000,1,f);
    fclose(f);
	/* only for the experiment
	buf1 = evbuffer_new();
	evbuffer_add(buf1,output,strlen(output));
	*/

    event_init();

    struct evhttp *httpd;
    httpd = evhttp_start("0.0.0.0",httpd_port);
    evhttp_set_timeout(httpd,httpd_timeout);

    evhttp_set_gencb(httpd,httpd_handler,NULL);

    event_dispatch();

    evhttp_free(httpd);
    return 0;
}
