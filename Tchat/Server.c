// Server.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h> 
#include <time.h>
#include <signal.h>

#define MAXREQ 100
#define MAXQUEUE 5
#define maxperson 100

struct packet{
  int consockfd_1;
  int consockfd_2;
  int* status;
};

struct consockfd_s{
	int consockfd;//unique id for chat 
	int status;//1 for chating 0 for free
};

int chat_counter=0;





// A utility function to swap to integers
void swap (int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void randomize ( int arr[], int n )
{
    // Use a different seed value so that we don't get same
    // result each time we run this program
    srand ( time(NULL) );
 
    // Start from the last element and swap one by one. We don't
    // need to run for the first element that's why i > 0
    for (int i = n-1; i > 0; i--)
    {
        // Pick a random index from 0 to i
        int j = rand() % (i+1);
 
        // Swap arr[i] with the element at random index
        swap(&arr[i], &arr[j]);
    }
}
void * client1(void *p){
 int n;
  struct packet *p1 = (struct packet *)p;
  char reqbuf[MAXREQ];
  while(1)
  {  
    memset(reqbuf,0, MAXREQ);
    n = read(p1->consockfd_2,reqbuf,MAXREQ-1); /* Recv */
    printf("value of n :: %d\n",n );
    write(p1->consockfd_1,reqbuf,strlen(reqbuf));
    if(n <=0){
    	*(p1->status) = 2;
    	return NULL; 
    }
  }
}
void * client2(void *p){
  int n;
  struct packet *p1 = (struct packet *)p;
  char reqbuf[MAXREQ];
 
  while(1)
  {  
    memset(reqbuf,0, MAXREQ);
    n = read(p1->consockfd_1,reqbuf,MAXREQ-1); /* Recv */
    printf("value of n :: %d\n",n );
    write(p1->consockfd_2,reqbuf,strlen(reqbuf));
    if(n <=0){
    	*(p1->status) = 1;
    	return NULL;
    }
    
  }
}
void server(int consockfd_1,int consockfd_2) {
 char welcomemsg[MAXREQ];
 int status = 0;
  pthread_t tid_send,tid_receive;
  strcpy(welcomemsg,"You are Now connected\n");
  write(consockfd_1,welcomemsg,strlen(welcomemsg));
  write(consockfd_2,welcomemsg,strlen(welcomemsg));
  if(1) {                   
    //receive
    struct packet *p_r = (struct packet*)malloc(sizeof(struct packet));
    p_r->consockfd_1 = consockfd_1;
    p_r->consockfd_2 = consockfd_2;
    p_r->status = &status;
    int r_r = pthread_create(&tid_receive, NULL,&client2,p_r);
    
    //send    
    struct packet *p_s = (struct packet*)malloc(sizeof(struct packet));
    p_s->consockfd_1 = consockfd_1;
    p_s->consockfd_2 = consockfd_2;
    p_s->status = &status;
    int r_s = pthread_create(&tid_send, NULL,&client1,p_s);
    
    while(status == 0);

    pthread_kill(tid_receive,0);
    pthread_kill(tid_send,0);

   	if(status == 1){
   		printf("close 1\n");
   		write(consockfd_2, "#404", 4);
   		close(consockfd_1);
   	}
   	else{
   		printf("close 2\n");
   		write(consockfd_1, "#404", 4);
   		close(consockfd_2);
   	}
    
    printf("Connection lost for pair !\n");
    pthread_exit(0);

   }
}
void * server_helper(void *p){
	struct packet *p1 = (struct packet *)p;
	server(p1->consockfd_1,p1->consockfd_2);
}
int anuk=0;
void randomPair(struct consockfd_s  consock_array[],int count_person){
	int i,j,jk=0;
	pthread_t tid[100];
	int *random_array = (int*) malloc(100* sizeof(int));
	//to-do
	//from 0  to count person, check status and allot 
	for (int i = 0; i < count_person; ++i)
	{
		if(consock_array[i].status==0){
			random_array[jk]=i;
			jk++;
		}
	}
	randomize(random_array,jk);
	i=0;
	j=jk;
	while(anuk < (jk)/2.0 && jk >= 2)
	{
		struct packet *p = (struct packet*)malloc(sizeof(struct packet));
	    p->consockfd_1 = consock_array[random_array[i+anuk]].consockfd;
	    consock_array[random_array[i+anuk]].status=1;
	    p->consockfd_2 = consock_array[random_array[j-anuk-1]].consockfd;
	    consock_array[random_array[j-anuk-1]].status=1;
	    printf("Paring :: %d %d\n",random_array[i+anuk],random_array[j-anuk-1]);
	    int r_s = pthread_create(&tid[chat_counter], NULL,&server_helper,p);
	    chat_counter++;
	    anuk++;
	}
	free(random_array);
	
	anuk=0;
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
while (count_person!=100) {
   printf("Listening for incoming connections\n");

/* Listen for incoming connections */
   listen(lstnsockfd, MAXQUEUE); 

   //clilen = sizeof(cl_addr);

/* Accept incoming connection, obtaining a new socket for it */
   consockfd = accept(lstnsockfd, (struct sockaddr *) &cli_addr,&clilen);
   if(consockfd > 0)
   {
   	  printf("Accepted connection %d\n",count_person);
   	  consock_array[count_person].consockfd = consockfd;
   	  consock_array[count_person].status = 0;
   	  consockfd = -404;
   	  count_person++;
   }

   
   
  randomPair(consock_array,count_person);

  // close(consockfd);
  }
  
close(lstnsockfd);
}
