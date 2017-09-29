// Server.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h> 

#define MAXREQ 20
#define MAXQUEUE 5

struct packet{
  int consockfd;
};

struct consockfd_s{
	int consockfd;//unique id for chat 
	int status;//1 for chating 0 for free
};

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
      printf("Recvd msg from sender:%s\n", reqbuf);
    // else 
    //   printf("ERROR\n");
  }
}
void server(int consockfd_1,int consockfd_1) {

  pthread_t tid_send,tid_receive;
  printf("IN server\n");
  if(1) {                   
    //receive
    struct packet *p_r = (struct packet*)malloc(sizeof(struct packet));
    p_r->consockfd = consockfd_1;
    int r_r = pthread_create(&tid_receive, NULL,&receive_t,p_r);
    
    //send    
    struct packet *p_s = (struct packet*)malloc(sizeof(struct packet));
    p_s->consockfd = consockfd_2;
    int r_s = pthread_create(&tid_send, NULL,&send_t,p_s);
    while(1){

    }

   }
}
void randomPair(struct consockfd_s  consock_array[],int count_person){
	int i,j;
	
	 server(consockfd[i].consockfd,consockfd[j].consockfd);
}
int main() {

int lstnsockfd, consockfd, clilen, portno = 5033;
struct sockaddr_in serv_addr, cli_addr;
struct consockfd_s consock_array[100];
int count_person = 0;

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
   if(consockfd = accept(lstnsockfd, (struct sockaddr *) &cli_addr,       
                      &clilen) && consockfd > 0)
   {
   	  printf("Accepted connection %d\n",count_person);
   	  consock_array[count_person] = consockfd;
   	  randomPair(consock_array,count_person);
   	  consockfd = -404;
   	  count_person++;
   }

   
   
  

  // close(consockfd);
  }
close(lstnsockfd);
}
