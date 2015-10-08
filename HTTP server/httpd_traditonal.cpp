#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define BUF_LEN 1028
#define SERVER_PORT 8000


const static char http_error_hdr[] = "HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\n\r\n";
const static char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
char http_index_html[8000000];


int http_send_file(char *filename, int sockfd)
{
  	if(!strcmp(filename, "/")){
        
    	write(sockfd, http_html_hdr, strlen(http_html_hdr));
    	write(sockfd, http_index_html, strlen(http_index_html));
  	}
  	else{
    	
    	//printf("%s:file not find!\n",filename);
    	write(sockfd, http_error_hdr, strlen(http_error_hdr));
  	}
  return 0;
}

void serve(int sockfd){
	char buf[BUF_LEN];
	read(sockfd, buf, BUF_LEN);
	if(!strncmp(buf, "GET", 3)){
		char *file = buf + 4;
    	char *space = strchr(file, ' ');
    	*space = '\0';
    	http_send_file(file, sockfd);
	}
	else{
		 
		//printf("unsupported request!\n");
		return;
	}
}
int sockfd;
void * thread(void * ptr)
{
    int * fd = (int*)ptr;
    int newfd = *fd;
    serve(newfd);
    close(newfd);
}
int main(){
	
	FILE * f = fopen("web/index.html","r");
	fread(http_index_html,8000000,1,f);
	fclose(f);
	
	
	int err,newfd;
	struct sockaddr_in addr;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		perror("socket creation failed!\n");
		return 0;
	}
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	
	addr.sin_port = htons(SERVER_PORT);
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))){
		perror("socket binding failed!\n");
		return 0;
	}
	listen(sockfd, 128);
	for(;;){
	    
		newfd = accept(sockfd, NULL, NULL);

		//serve(newfd);
		//close(newfd);
        pthread_t tid1;
        pthread_create(&tid1,NULL,thread,&newfd);
	}
	return 0;
}
