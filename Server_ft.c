// Server.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#define MAXREQ 20
#define MAXQUEUE 50

void server(int consockfd) {
  char reqbuf[MAXREQ];
  int filedesc = open("received.txt", O_CREAT, S_IXUSR);
  int n;
  while (1) {                   
    memset(reqbuf,0, MAXREQ);
    n = read(consockfd,reqbuf,MAXREQ-1); /* Recv */
    
    if (n > 0)
    {
    printf("Recvd msg:%s\n", reqbuf);
     write(filedesc, reqbuf, strlen(reqbuf)); /* echo*/
    }
    else
      return;
  }
}

int main() {
int lstnsockfd, consockfd, clilen, portno = 5033;
struct sockaddr_in serv_addr, cli_addr;

 memset((char *) &serv_addr,0, sizeof(serv_addr));
 serv_addr.sin_family      = AF_INET;
 serv_addr.sin_addr.s_addr = INADDR_ANY;
 serv_addr.sin_port        = htons(portno);

// Server protocol
/* Create Socket to receive requests*/
lstnsockfd = socket(AF_INET, SOCK_STREAM, 0);

/* Bind socket to port */
bind(lstnsockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr));
printf("Bounded to port\n");
while (1) {
   printf("Listening for incoming connections\n");

/* Listen for incoming connections */
   listen(lstnsockfd, MAXQUEUE); 

   //clilen = sizeof(cl_addr);

/* Accept incoming connection, obtaining a new socket for it */
   if(consockfd = accept(lstnsockfd, (struct sockaddr *) &cli_addr,&clilen)){
    printf("Accepted connection\n");
    server(consockfd);
    }
   close(consockfd);
   break;
  }
close(lstnsockfd);
}
