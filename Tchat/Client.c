// Client.c
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h> 
#include <signal.h>

#define MAXIN 100
#define MAXOUT 100
#define ERROR "#404"

struct packet{
  int sockfd;
};

void *send_t(void *p) {
  char sndbuf[MAXIN];/* Get request char stream */
  struct packet *p1 = (struct packet *)p;
  while(1)
  { 
  	memset(sndbuf,0,MAXIN);
    fgets(sndbuf,MAXIN,stdin);
    write(p1->sockfd, sndbuf, strlen(sndbuf)); /* send */
  }
}
void *receive_t(void *p) {
   int n;
  char rcvbuf[MAXOUT];/* Get request char stream */
  char Info[50];
  strcpy(Info,"Random Person:");
 struct packet *p1 = (struct packet *)p;
  while(1)
  {
    memset(rcvbuf,0,MAXOUT);               /* clear */
    n=read(p1->sockfd, rcvbuf, MAXOUT-1);      /* receive */

  	if(!strcmp(rcvbuf, ERROR))
    	return NULL;
    
    if(n>0)
    {
      strcpy(Info,"Random Person:");
      strcat(Info,rcvbuf);
      printf("%s",Info);
      fflush(stdout);
      //write(STDOUT_FILENO, rcvbuf, n);        /* echo */
    }
    else
    	return NULL;

  }
}

void client(int sockfd) {
  char rcvbuf[MAXOUT];
  pthread_t tid_send,tid_receive;

   struct packet *p_r = (struct packet*)malloc(sizeof(struct packet));
   p_r->sockfd = sockfd;
   int r_r = pthread_create(&tid_send, NULL,&send_t,p_r);
  
    struct packet *p_s = (struct packet*)malloc(sizeof(struct packet));
    p_s->sockfd = sockfd;
    int r_s = pthread_create(&tid_receive, NULL,&receive_t,p_s);

   pthread_join(tid_receive,NULL);
   pthread_kill(tid_send,0);

   printf("Finding Other Person For You...\n");
}

// Server address
struct hostent *buildServerAddr(struct sockaddr_in *serv_addr,
	char *serverIP, int portno) {
  /* Construct an address for remote server */
  memset((char *) serv_addr, 0, sizeof(struct sockaddr_in));
  serv_addr->sin_family = AF_INET;
  inet_aton(serverIP, &(serv_addr->sin_addr));
  serv_addr->sin_port = htons(portno);
 }


int main() {
	//Client protocol
	char *serverIP = "10.8.12.48";
	int sockfd, portno = 5033;	struct sockaddr_in serv_addr;
	
	buildServerAddr(&serv_addr, serverIP, portno);

	/* Create a TCP socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	while(1){
	/* Connect to server on port */
	if(!connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)))
	  {
	   printf("Connected to %s:%d\n",serverIP, portno);
	   client(sockfd);
	  }

	/* Carry out Client-Server protocol */
	

	}
	/* Clean up on termination */
	close(sockfd);
}