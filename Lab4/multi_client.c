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


// Client function for the threads
void *clientfunction(void * threadid){
		/* connect to server */
	char buffer[256];
	char recvBuff[512];
	int bytesReceived=0;
	int sockfd;
	time_t seconds;
	int n;
	memset(buffer, '\0', sizeof(buffer));		
	// Start time for a thread
	seconds = time(NULL);

	int id = (int)threadid;
	// Checks for duration over
	while(time(NULL)-seconds<timesec)
	{
		// Increase the count of files for each thread
		counter_array[id]++;
		/* Attempt a connection */
		int second1 = (int)time(NULL);
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) error("ERROR opening socket");
		if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
		{
			printf("\n Error : Connect Failed \n");
			return 1;
		}



		if (mode == 0)
		{
			// strcpy(buffer,"/tmp/files/foo0.txt");
			strcpy(buffer,"get files/foo0.txt");
		}
		else
		{
			// Generate a random number for a random file
			int random_number = rand()%10000;
			char str[] = "get files/foo" ;
			char str1[242];
			sprintf(str1,"%d",random_number);
			strcat(str, str1);
			strcat(str, ".txt");
			strcpy(buffer,str);																																																																																																																																																																																																																																				
		}

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
    		// printf("%s", recvBuff);
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

    timer_array[id] += time_in_mill_end - time_in_mill_start;
    sleep(thinktime);

}
pthread_exit(NULL);
}


int main(int argc, char *argv[])
{


	if (argc < 7) {
		fprintf(stderr,"usage %s <hostname> <port> <number_of_threads> <Time_for_threads_to_run> <Sleep_time> <mode>\n", argv[0]);
		exit(0);
	}

		/* create socket, get sockfd handle */

	portno = atoi(argv[2]);
	concurrent = atoi(argv[3]);
	timesec = atoi(argv[4]);
	thinktime = atoi(argv[5]);
	if(strcmp(argv[6],"random")==0){
		mode = 1;
	}

	counter_array = malloc(sizeof(int) * concurrent);
	memset(counter_array, 0, concurrent * sizeof(int));

	timer_array = malloc(sizeof(int) * concurrent);
	memset(timer_array, 0, concurrent * sizeof(int));


	/* fill in server address in sockaddr_in datastructure */

	server = gethostbyname(argv[1]);
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
	// Create threads
	for (i = 0; i < concurrent; ++i)
	{
		rc = pthread_create(&(threads[i]), NULL, 
			clientfunction, (void *)i);
		if (rc){
		 // cout << "Error:unable to create thread," << rc << endl;
			exit(-1);
		}
	}
	i=0;
	void * status;
	for (i = 0; i < concurrent; ++i)
	{
		rc = pthread_join(threads[i], &status);
		if (rc){
			exit(-1);
		}
	}
	gettimeofday(&tot_tv1, NULL);
	// time for start and end of experiment
	double total_time_end = (tot_tv1.tv_sec) * 1000 + (tot_tv1.tv_usec) / 1000 ;

	int sum=0;
	double sum_time = 0;
	for(i=0; i<concurrent; i++) 
	{	
		sum += counter_array[i];
	}

	for(i=0; i<concurrent; i++) 
	{	
		sum_time += timer_array[i];		
	}


	double total_time = total_time_end - total_time_start;

	printf("Done!\n");

	double throughput = (sum*1000)/total_time;
	printf("throughput = %lf req/s\n",throughput);

	double response = sum_time/(1000*sum);
	printf("average response time = %lf sec\n",response);

	// Free the Space picked up from the Heap Memory
	free(timer_array);
	free(counter_array);

	return 0;
}