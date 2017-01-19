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
#include <pthread.h>
#include <queue>
using namespace std;
queue<int> A;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queuehasdata = PTHREAD_COND_INITIALIZER;
pthread_cond_t queuenotfull = PTHREAD_COND_INITIALIZER;
// threadfunction
void * threadfunction(void * i){
	while(1){		
	// lock held so as to update the shared data structure
	pthread_mutex_lock(&mutex);
	while(A.size()<=0){
		// If no request then  wait
		pthread_cond_wait(&queuehasdata, &mutex);
	}
	// See the front element of queue
	int connfd = A.front();
	// Remove from queue
	A.pop();
	// If received the request signal new request can come
	pthread_cond_signal(&queuenotfull);
	// Unlock
	pthread_mutex_unlock(&mutex);
	// Code as in previous one for reading and writing
	char buffer[512];
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
	FILE *fp = fopen(str,"rb");
	if(fp==NULL)
	{
	    printf("File open error");
	    // return 1;
	    close(connfd);
		continue;
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
	        
	        break;
	    }

	}
	fclose(fp);
			close(connfd);
}

}


int main(int argc, char *argv[])
{
	int listenfd = 0;
	int connfd = 0;
	struct sockaddr_in serv_addr;
	char buffer[512];
	char sendBuff[1024];
	int queuesize = atoi(argv[3]);
	int number = atoi(argv[2]);
	pthread_t threads[number];
	int numrv;
	int port;
	// signal(SIGCHLD,chld_termination_sig_handler);
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
	// Create threads given in input
	for (i = 0; i < number; ++i)
	{
		pthread_create(&threads[i],NULL,threadfunction,(void *)i);
	}

	if(listen(listenfd, 2000) == -1)
	{
		printf("Failed to listen\n");
		return -1;
	}
	// processarray = (int *)malloc(queuesize*sizeof(int));
	// counter = 0;
	while(1)
	{
		// Accept connection
		connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL);
		// Lock to update the queue
		pthread_mutex_lock(&mutex);
		// If queue already full
		while(A.size()>=queuesize && queuesize!=0){
			pthread_cond_wait(&queuenotfull, &mutex);
		}
		// Push the socket into the queue
		A.push(connfd);
		// Signal that the queue has data
		pthread_cond_signal(&queuehasdata);
		pthread_mutex_unlock(&mutex);
		// printf("Connfd%d\n", connfd);
	}
	// free(processarray);


	return 0;
}