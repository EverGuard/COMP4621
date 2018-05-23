#include <stdio.h>
  #include <string.h>
  #include <sys/socket.h>
  #include <sys/types.h>
  #include <netinet/in.h>
  #include <time.h>
  #include <pthread.h>
  
  #include <sys/stat.h>
  #include <fcntl.h>
    
  #define SERVER_PORT (12345)
  #define LISTENNQ (5)
  #define MAXLINE (5000)
  #define MAXTHREAD (999)
  #define r_http "HTTP/1.1 200 OK\\r\\n\n"
  #define r_connect "Connection: Keep-Alive\\r\\n\n\\r\\n\n"
  
  
  char webpage[] =
  "HTTP/1.1 200 OK\r\n"
  "Content-Type: text/html; charset=UTF-8\r\n\r\n"
  "<!DOCTYPE html>\r\n"
  "<html><head><title>Hello</title>\r\n"
  "<style>body { background-color: #FFFF00 }</style></head>\r\n"
  "<body><center><h1>hello world</h1><br>\r\n"
  "<img src=\"test1.jpg\"></center></body></html>\r\n";

  void* request_func(void *args);
  
            //added
	  char buf[2048];
	  int fdimg;
	  int on=1;
	  //added

  int main(int argc, char **argv)
  {	  	  
	  int listenfd, connfd;

          struct sockaddr_in servaddr, cliaddr;
          socklen_t len = sizeof(struct sockaddr_in);
  

          char ip_str[INET_ADDRSTRLEN] = {0};
  

          int threads_count = 0;
          pthread_t threads[MAXTHREAD];
  

  

	  
          /* initialize server socket */
          listenfd = socket(AF_INET, SOCK_STREAM, 0); /* SOCK_STREAM : TCP */
          if (listenfd < 0) {
                  printf("Error: init socket\n");
                  return 0;
          }
          /* initialize server address (IP:port) */
          memset(&servaddr, 0, sizeof(servaddr));
          servaddr.sin_family = AF_INET;
          servaddr.sin_addr.s_addr = INADDR_ANY; /* IP address: 0.0.0.0 */
          servaddr.sin_port = htons(SERVER_PORT); /* port number */
          /* bind the socket to the server address */
          if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) < 0) {
                  printf("Error: bind\n");
                  return 0;
          }
          if (listen(listenfd, LISTENNQ) < 0) {
                  printf("Error: listen\n");
                  return 0;
          }
  

  
  
          /* keep processing incoming requests */
          while (1) {
                  /* accept an incoming connection from the remote side */
                  connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);
                  if (connfd < 0) {
                          printf("Error: accept\n");
                          return 0;
                  } 

                  
                  
                  /* print client (remote side) address (IP : port) */
                  inet_ntop(AF_INET, &(cliaddr.sin_addr), ip_str, INET_ADDRSTRLEN);
                  printf("Incoming connection from %s : %hu with fd: %d\n", ip_str, ntohs(cliaddr.sin_port), connfd);
  

		  
		  
                  /* create dedicate thread to process the request */
                  if (pthread_create(&threads[threads_count], NULL, request_func, (void *)connfd) != 0) {
                          printf("Error when creating thread %d\n", threads_count);
                          return 0;
                  }
  

                  if (++threads_count >= MAXTHREAD) {
                          break;
                  }
  

          }
          printf("Nax thread number reached, wait for all threads to finish and exit...\n");
	  int i;
          for (i = 0; i < MAXTHREAD; ++i) {
                  pthread_join(threads[i], NULL);
          }
  

          return 0;
  }
  

  void* request_func(void *args)
  {
          /* get the connection fd */
	  int file_len=0;
	  char str_file_len[MAXLINE]={0};
          int connfd = (int)args;
          char buff[MAXLINE] = {0};
	  char response[6000]={0};
	  char r_file[MAXLINE]={0};
	  char fbuf[4096];
	  
	  strcpy(response, r_http);	  
	  FILE *fp = NULL;
	  
	  memset(buf, 0, 2048);
	  read(connfd, buf, 2047);
	  //printf("%s\n", buf);

          printf("heavy computation\n");
             
		  
	  if(!strncmp(buf, "GET /test1.jpg", 10)) {
		fp = fopen("test1.jpg", "r");
		fseek(fp, 0, SEEK_END);
		file_len=ftell(fp);
		sprintf(str_file_len, "%d" ,file_len);
		fseek(fp, 0, SEEK_SET);
		fread(fbuf, file_len+1, 1, fp);
		fclose(fp);
		
		fdimg = open("test1.jpg",O_RDONLY);
		sendfile(connfd, fdimg, NULL,8000);
		close(fdimg);	
		
		strcat(response, "Content-Type: image/jpeg\\r\\n\n");
		strcat(response, "Content-Length: ");
		strcat(response, str_file_len);
		strcat(response, r_connect);
		printf(response);
		printf("JPG file content:\n%s\n", fbuf);
	  }
	  
	 else if(!strncmp(buf, "GET /test3.css", 10)) {		
		fp = fopen("test3.css", "r");
		fseek(fp, 0, SEEK_END);
		file_len=ftell(fp);
		sprintf(str_file_len, "%d" ,file_len);
		fseek(fp, 0, SEEK_SET);
		fread(fbuf, file_len+1, 1, fp);
		fclose(fp);
		
		fdimg = open("test3.css",O_RDONLY);
		sendfile(connfd, fdimg, NULL,100);
		close(fdimg);	
		
		strcat(response, "Content-Type: text/css\\r\\n\n");
		strcat(response, "Content-Length: ");
		strcat(response, str_file_len);
		strcat(response, r_connect);
		printf(response);
		printf("CSS file content:\n%s\n", fbuf);
	  }
	  
	  else if(!strncmp(buf, "GET /test2.pdf", 10)) {		
		fp = fopen("test2.pdf", "r");
		fseek(fp, 0, SEEK_END);
		file_len=ftell(fp);
		sprintf(str_file_len, "%d" ,file_len);
		//fseek(fp, 0, SEEK_SET);
		//fread(fbuf, file_len+1, 1, fp);		
		fclose(fp);
		
		fdimg = open("test2.pdf",O_RDONLY);
		sendfile(connfd, fdimg, NULL,40000);
		close(fdimg);	
		
		strcat(response, "Content-Type: application/pdf\\r\\n\n");
		strcat(response, "Content-Length: ");
		strcat(response, str_file_len);
		strcat(response, r_connect);
		printf(response);
		//printf("PDF file content:\n%s\n", fbuf);
	  }	  
	  
	  else if(!strncmp(buf, "GET / HTTP/1.1", 10)) {
          /* prepare for the send buffer */ 
          //snprintf(buff, sizeof(buff) - 1, webpage, connfd);
          /* write the buffer to the connection */
		fp = fopen("test4.html", "r");
		fseek(fp, 0, SEEK_END);
		file_len=ftell(fp);
		sprintf(str_file_len, "%d" ,file_len);
		fseek(fp, 0, SEEK_SET);
		fread(fbuf, file_len+1, 1, fp);
		fclose(fp);
		
		fdimg = open("test4.html",O_RDONLY);
		sendfile(connfd, fdimg, NULL,500);
		close(fdimg);	
		
		strcat(response, "Content-Type: text/html\\r\\n\n");
		strcat(response, "Content-Length: ");
		strcat(response, str_file_len);
		strcat(response, r_connect);
		printf(response);
		printf("HTML file content:\n%s\n", fbuf);
          //write(connfd, webpage, strlen(webpage)-1);
	  }
	  
	  else{
		fp = fopen("error.html", "r");
		fseek(fp, 0, SEEK_END);
		file_len=ftell(fp);
		sprintf(str_file_len, "%d" ,file_len);
		fseek(fp, 0, SEEK_SET);
		fread(fbuf, file_len+1, 1, fp);
		fclose(fp);
		
		fdimg = open("error.html",O_RDONLY);
		sendfile(connfd, fdimg, NULL,500);
		close(fdimg);	
	  
		strcat(response, "Content-Type: text/html\\r\\n\n");
		strcat(response, "Content-Length: ");
		strcat(response, str_file_len);
		strcat(response, r_connect);
		printf(response);
		printf("HTML file content:\n%s\n", fbuf);
	  }
	  	  
          close(connfd);
	  printf("closing tab.\n");
  }