#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>
void error(char *msg)
{
	perror(msg);
	exit(0);
}

struct sockaddr_in serv_addr;
struct hostent *server;
int portno, n, concurrent, timesec, thinktime;
int mode;
// int count=0;
int* counter_array;
int* timer_array;
double time_in_mill_start=0.0;
double time_in_mill_end=0.0;
struct timeval  tv,tv1;
struct timeval  tot_tv, tot_tv1;


int main(int argc, char *argv[])
{


	if (argc < 5) {
		fprintf(stderr,"usage %s <filetofetch> <hostname> <port> <displaymode>\n", argv[0]);
		exit(0);
	}

		/* create socket, get sockfd handle */

	portno = atoi(argv[3]);
	if(strcmp(argv[6],"random")==0){
		mode = 1;
	}


	/* fill in server address in sockaddr_in datastructure */

	server = gethostbyname(argv[2]);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
		(char *)&serv_addr.sin_addr.s_addr,
		server->h_length);
	serv_addr.sin_port = htons(portno);

	// Define concurrent number of threads
	pthread_t threads[concurrent];
	int rc;
	int i = 0;
	gettimeofday(&tot_tv, NULL);
	double total_time_start = (tot_tv.tv_sec) * 1000 + (tot_tv.tv_usec) / 1000 ;
	// The client reads and sends here
//*****************************************************

	char buffer[256];
	char displaymode[256];
	char recvBuff[512];
	int bytesReceived=0;
	int sockfd;
	time_t seconds;
	int n;
	memset(buffer, '\0', sizeof(buffer));		
	// Start time for a thread
	seconds = time(NULL);

	// Checks for duration over
	// Increase the count of files for each thread
	/* Attempt a connection */
	int second1 = (int)time(NULL);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) error("ERROR opening socket");
	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
	{
		printf("\n Error : Connect Failed \n");
		return 1;
	}
	bzero(buffer,256);
	strcat(buffer,"get ");
	strcat(buffer,argv[1]);
	printf("%s\n", buffer);
	strcpy(displaymode,argv[4]);

		// start time for the thread
	gettimeofday(&tv, NULL);
	time_in_mill_start = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;

	n = write(sockfd,buffer,strlen(buffer)+1);
	if(n<=0)
	{
		printf("Error in Sending File Name\n");
	}

	


    /* Receive data in chunks of 512 bytes */
	while(1)
	{
		memset(recvBuff, '\0', sizeof(recvBuff));
		bytesReceived = read(sockfd, recvBuff, 511);
		if(bytesReceived > 0)
		{
			if(strlen(displaymode)==7){
				printf("%s", recvBuff);
			}
		}
		else if(bytesReceived <= 0)
		{
			break;
		}    
		else if(bytesReceived == 0)
		{
			
			break;
		}	
		else printf("Error \n");
	}

	int second2 = (int)time(NULL);
    // Time spent on writing the file
    // timer_array[id] += second2-second1;
	close(sockfd);
	gettimeofday(&tv1, NULL);
	time_in_mill_end = (tv1.tv_sec) * 1000 + (tv1.tv_usec) / 1000 ;




//*****************************************************


	gettimeofday(&tot_tv1, NULL);
	// time for start and end of experiment
	double total_time_end = (tot_tv1.tv_sec) * 1000 + (tot_tv1.tv_usec) / 1000 ;


	return 0;
}