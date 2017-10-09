// Server.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h> 
#include <semaphore.h>

#define MAXREQ 20
#define MAXQUEUE 5

struct packet{
  int consockfd;
};

sem_t semvar;

void * send_t(void *p){
  char sendbuf[MAXREQ];
  struct packet *p1 = (struct packet *)p;
    printf("IN Sender\n");
  while(1)
  {  
    scanf("%s",sendbuf);
    write(p1->consockfd,sendbuf,strlen(sendbuf));
  }
}
void * receive_t(void *p){
  int n;
  struct packet *p1 = (struct packet *)p;
  char reqbuf[MAXREQ];
  printf("IN Reciver\n");
  while(1)
  {  
    memset(reqbuf,0, MAXREQ);
    n = read(p1->consockfd,reqbuf,MAXREQ-1); /* Recv */
    if(n>0)
    {
      sem_wait(&semvar);
      printf("Recvd msg from sender:%s\n", reqbuf);
      sem_post(&semvar);
    }
    // else 
    //   printf("ERROR\n");
  }
}
void server(int consockfd) {

  pthread_t tid_send,tid_receive;
  printf("IN server\n");
  if(1) {                   
    //receive
    struct packet *p_r = (struct packet*)malloc(sizeof(struct packet));
    p_r->consockfd = consockfd;
    int r_r = pthread_create(&tid_receive, NULL,&receive_t,p_r);
    
    //send    
    struct packet *p_s = (struct packet*)malloc(sizeof(struct packet));
    p_s->consockfd = consockfd;
    int r_s = pthread_create(&tid_send, NULL,&send_t,p_s);
    while(1){

    }

   }
}

int main() {

int lstnsockfd, consockfd, clilen, portno = 5033;
struct sockaddr_in serv_addr, cli_addr;

 memset((char *) &serv_addr,0, sizeof(serv_addr));
 serv_addr.sin_family      = AF_INET;
 serv_addr.sin_addr.s_addr = INADDR_ANY;
 serv_addr.sin_port        = htons(portno);
sem_init(&semvar, 0, 1);

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
   consockfd = accept(lstnsockfd, (struct sockaddr *) &cli_addr,       
                      &clilen);
   
   printf("Accepted connection\n");

   server(consockfd);

   close(consockfd);
  }
close(lstnsockfd);
}
