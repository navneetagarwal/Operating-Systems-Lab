#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

void chld_termination_sig_handler(int sig)
{
	// Signal handler
	while(1)
	{		
		pid_t p;		
		int status;
		p = waitpid(-1, &status, WNOHANG|WUNTRACED);		
		if (p == -1){
		   /* continue on interruption (EINTR) */
			   if (errno == EINTR) {
			       continue;
			}
		}	
    	if (p == 0) 
		{
			//No Child has exited
		}		
		if (p > 0) 
		{
			// printf("exited, status=%d\n", p);
			return;
		}
	}
}



int main(int argc, char *argv[])
{
    int listenfd = 0;
    int connfd = 0;
    struct sockaddr_in serv_addr;
    char buffer[512];
    char sendBuff[1024];
    int numrv;
    int port;
	signal(SIGCHLD,chld_termination_sig_handler);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0)
	{
		printf("Socket Couldn't be Opened, Sorry Rahega\n");
		exit(EXIT_FAILURE);
	}
    
	int yes = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
    	perror("setsockopt");
   	 	exit(1);
	}

    // printf("Socket retrieve success\n");
    pid_t arr[1024];
	memset(arr, -1, sizeof(pid_t)*1024);

	int alpha=0;

    int i;
    int k = 0;

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));

    if(listen(listenfd, 2000) == -1)
    {
        printf("Failed to listen\n");
        return -1;
    }


    while(1)
    {
	connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL);
	pid_t ret;
	ret = fork();
	if(ret==0){	
		bzero(buffer,512);
		int tn = read(connfd,buffer,512);
		if(tn <= 0)
		{
			printf("Error in Receiving\n");
		}
		char str[512];
		bzero(str, 512);
		int j = 4;
		for(j=4;buffer[j]!='\0';j++){
			str[j-4]=buffer[j];
		}
		str[j]='\0';
		printf("%s\n", str);
		FILE *fp = fopen(str,"rb");
		if(fp==NULL)
		{
		    printf("File open error");
		    return 1;
		}
		/* Read data from file and send it */
		while(1)
		{
		    /* First read file in chunks of 512 bytes */
		    unsigned char buff[512]={0};
		    int nread = fread(buff,1,512,fp);
		    // printf("Bytes read %d \n", nread);        

		    /* If read was success, send data. */
		    if(nread > 0)
		    {
		        // printf("Sending \n");
		        write(connfd, buff, nread);
		        sleep(1);
		    }

		    /*
		     * There is something tricky going on with read .. 
		     * Either there was error, or we reached end of file.
		     */
		    if (nread < 512)
		    {
		        if (feof(fp))
		            // printf("End of file\n");
		        if (ferror(fp))
		            printf("Error reading\n");
				close(connfd);
		        break;
		    }
		    
		}
	//The child process is done
	exit(0);	
    }
    else{
	    close(connfd);
	}
        
        sleep(0);
    }


return 0;
}