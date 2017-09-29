#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char rcvbuf[100];
    memset(rcvbuf,0,100);               /* clear */
    scanf("%s",rcvbuf);                   /* receive */
    printf("Output: %s",rcvbuf);
    write(STDOUT_FILENO, rcvbuf, (strlen(rcvbuf)));          /* echo */
    
    return 0;
}