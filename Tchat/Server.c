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
#define ERROR "#404"
#define NEXT "#405"

struct pair{
  int client1;
  int client2;
  struct consockfd_s *consock_array;
  int* status;
};

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
struct consockfd_s consock_array[100];
int count_person = 0;




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
    if(!strcmp(reqbuf,NEXT)){
    	*(p1->status) = 3;
    	return NULL; 
    }
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
    if(!strcmp(reqbuf,NEXT)){
    	*(p1->status) = 4;
    	return NULL; 
    }
   write(p1->consockfd_2,reqbuf,strlen(reqbuf));
    if(n <=0){
    	*(p1->status) = 1;
    	return NULL;
    }
    
  }
}
void server(int client_1,int client_2,struct consockfd_s consock_array[]) {
 char welcomemsg[MAXREQ];
 int status = 0;
  pthread_t tid_send,tid_receive;
  int consockfd_1 = consock_array[client_1].consockfd;
  int consockfd_2 = consock_array[client_2].consockfd;
  strcpy(welcomemsg,"is now connected\n");
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

    pthread_cancel(tid_receive);
    pthread_cancel(tid_send);

   	if(status == 1){
   		printf("closed %d\n",client_1);
   		write(consockfd_2, ERROR, 4);
   		printf("Enabling client %d for another chat\n",client_2);
   		consock_array[client_2].status = 0;
   		consock_array[client_1].status = 2;//2 means gone foreever
   		close(consockfd_1);
   	}
   	else if (status == 2){
   		printf("closed %d\n",client_2);
   		write(consockfd_1, ERROR, 4);
   		printf("Enabling client %d for another chat\n",client_1);
   		consock_array[client_1].status = 0;
   		consock_array[client_2].status = 2;//2 means offline
   		close(consockfd_2);
   	}
   	else if(status == 3){
   		printf("Enabling client %d %d for some another chat\n",client_1,client_2);
   		write(consockfd_1, ERROR, 4);
   		consock_array[client_1].status = 0;
   		consock_array[client_2].status = 0;
   	}
   	else{
   		printf("Enabling client %d %d for some another chat\n",client_1,client_2);
   		write(consockfd_2, ERROR, 4);
   		consock_array[client_1].status = 0;
   		consock_array[client_2].status = 0;
   	}
    printf("**************************************\n");
    printf("Index\tconsock_array index \t status \n");
    for (int i = 0; i < count_person; ++i)
    {
    	printf("%d\t\t\t%d\t\t\t%d\n",i,consock_array[i].consockfd,consock_array[i].status);
    }
    //printf("Connection lost for pair !\n");
    pthread_exit(0);

   }
}
void * server_helper(void *p){
	struct pair *p1 = (struct pair *)p;
	server(p1->client1,p1->client2,p1->consock_array);
}

void * randomPair(){
	while(1)
	{
		int start_index,last_index,free_person=0,middle;
		int inc=0;
		pthread_t tid[100];
		int *random_array = (int*) malloc(100* sizeof(int));
		//to-do
		//from 0  to count person, check status and allot 
		for (int i = 0; i < count_person; ++i)
		{
			if(consock_array[i].status==0){
				random_array[free_person]=i;
				free_person++;
			}
		}
		randomize(random_array,free_person);
		start_index=0;
		last_index=free_person-1;
		if(free_person%2==0)
			{
				middle  = (free_person)/2 ;
			}
		else
			{
				middle = (free_person)/2 ;
			}

		while(free_person >= 2 && inc < middle  )
		{

			struct pair *p = (struct pair*)malloc(sizeof(struct pair));
		    p->client1 = random_array[start_index + inc];
		    consock_array[random_array[start_index + inc]].status=1;
		    p->client2 = random_array[last_index-inc];
		    consock_array[random_array[last_index-inc]].status=1;
		    p->consock_array = consock_array;
		    printf("Paring :: %d %d\n",random_array[start_index + inc],random_array[last_index-inc]);
		    int r_s = pthread_create(&tid[chat_counter], NULL,&server_helper,p);
		    chat_counter++;
		    inc++;
		}
		free(random_array);
		
		inc=0;
	}
}


int main() {

int lstnsockfd, consockfd, clilen, portno = 5033;
struct sockaddr_in serv_addr, cli_addr;

pthread_t tid_random;
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


int randomprocess_thread = pthread_create(&tid_random, NULL,&randomPair,NULL);
if(randomprocess_thread < 0)
	printf("Failed to create randomizing pairing thread! \n");

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

   	
   
  

  // close(consockfd);
  }
  
close(lstnsockfd);
}
