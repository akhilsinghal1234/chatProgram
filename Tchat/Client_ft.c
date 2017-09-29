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

#define MAXIN 20000
#define MAXOUT 20000

char* readFileBytes(FILE *fl)  
{  
   
    fseek(fl, 0, SEEK_END);  
    long len = ftell(fl);  
    char *ret = malloc(len);  
    fseek(fl, 0, SEEK_SET);  
    fread(ret, 1, len, fl);  
    fclose(fl);  
    return ret;  
}

char *getreq(char *inbuf, int len) {
  /* Get request char stream */
  printf("REQ: ");              /* prompt */
  memset(inbuf,0,len);          /* clear for good measure */
  return fgets(inbuf,len,stdin); /* read up to a EOL */
}

void client(int sockfd,FILE *fp) {
  int n;
  char sndbuf[MAXIN]; char rcvbuf[MAXOUT];
  strcpy(sndbuf,readFileBytes(fp));        /* prompt */
  printf("sending msg:%s\n", sndbuf);
  if (strlen(sndbuf) > 0) {
    write(sockfd, sndbuf, strlen(sndbuf)); /* send */

    //printf("Wrote message: %s\n",sndbuf);
    
    // memset(rcvbuf,0,MAXOUT);               /* clear */
    // n=read(sockfd, rcvbuf, MAXOUT-1);      /* receive */
    // //printf("Received reply: %d",n);
    
    // write(STDOUT_FILENO, rcvbuf, n);	      /* echo */
    // getreq(sndbuf, MAXIN);                 /* prompt */
  }
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
	int sockfd, portno = 5033;
	struct sockaddr_in serv_addr;

  char filename[50];
  printf("Please enter file name to be send !");
	scanf("%s",filename);
  FILE *fp;
  fp = fopen(filename,"r");

	buildServerAddr(&serv_addr, serverIP, portno);

	/* Create a TCP socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	/* Connect to server on port */
	if(!connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)))
	   printf("Connected to %s:%d\n",serverIP, portno);
  else
     printf("Not connected !"); 
	/* Carry out Client-Server protocol */
	client(sockfd,fp);

	/* Clean up on termination */
	close(sockfd);
}