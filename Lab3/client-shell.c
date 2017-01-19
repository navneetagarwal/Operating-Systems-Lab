#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64



int chdir(const char *path);


int * pid_array;																// Pid array for foreground processes
int * pid_array_background;														// Pid array for foreground processes
int countpid;																	// Count of foreground processes
int countpid_background;														// Count of background processes
int sq;																			// Counter for getsq to stop when SIGINT comes
char **tokenize(char *line)														// Function for tokenizing
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

void signal_controlc(){															// SIGINT function

	sq = 1;																		// Dont allow further downloads getsq
	printf("\n");					

}

void chld_termination_sig_handler(){											// SIGCHLD function
	int status;														
	int p = waitpid(-1, &status, WNOHANG|WUNTRACED);							
	int i1;
	int f = 0;
	int index = -1;
	for (i1 = 0; i1 < countpid_background; ++i1)								// See if its a background process
	{	
		if(pid_array_background[i1]==p){
			f = 1;
			index = i1;
			break;
		}
	}
	if(f == 1){			
		pid_array_background[index] = pid_array_background[countpid_background-1];
		countpid_background--;													
		printf("Background process Finished\n");
	}
}

void  main(void)
{
     char  line[MAX_INPUT_SIZE];            
     char  **tokens;              
     int i;
     char cwd[1024];
     char IP[1024];
     char port[1000];
     int port_IP = 0;
     sq = 0;
     pid_array_background = (int *)malloc(sizeof(int)*MAX_INPUT_SIZE);
     countpid = 0;
     countpid_background = 0;
	 signal(SIGCHLD,chld_termination_sig_handler);
     signal(SIGINT, signal_controlc);
     while (1) {           
       
       printf("Hello>");     													// print hello on screen
       bzero(line, MAX_INPUT_SIZE);
       gets(line);           
       line[strlen(line)] = '\n'; //terminate with new line
       tokens = tokenize(line);
   
       //do whatever you want with the commands, here we just print them
       int count = 0;
       for(i=0;tokens[i]!=NULL;i++){
       		 count++;
	         // printf("found token %s\n", tokens[i]);
       }

       if(tokens[0]==NULL){
       }
       else{
			       if(strcmp(tokens[0],"exit")==0){								// exit on pressing exit 

			       		killpg(0, SIGINT);										// kill background processes
			       		int i1 = 0;												
			       		for (i1 = 0; i1 < countpid_background; ++i1)			// kill the processes pid in the array
			       		{
			       			kill(pid_array_background[i1],SIGINT);
			       		}
			       		free(pid_array_background);
			       		// Freeing the allocated memory	
				        for(i=0;tokens[i]!=NULL;i++){
					       free(tokens[i]);
				       	}
				       	free(tokens);
			       		exit(1);
			       }
			       
			       if(strcmp(tokens[0],"cd")==0){								// change directory
			       		if(count>2 || count==1){
			       			printf("Error in the number of arguments\n");
			       		}
			       		else{
			       			errno = 0;
							chdir(tokens[1]);
							if ( errno != 0 ) {
							    printf( "Error changing dir: %s\n", strerror( errno ) );// if no such directory exists
							}
							else{
								if (getcwd(cwd, sizeof(cwd)) != NULL)
						        	fprintf(stdout, "Current working dir: %s\n", cwd);	
						   		else
						       		perror("getcwd() error");
							}
			       		}
			       }
			       else if(strcmp(tokens[0],"server")==0){								// Store the port and IP
			       		if(count!=3){	
			       			printf("Error in the number of arguments\n");
			       		}
			       		else{
			       			strcpy(IP,tokens[1]);
			       			strcpy(port, tokens[2]);
			       			port_IP = 1;
			       		}
			       }
			       else if(strcmp(tokens[0],"getfl")==0){
			       		if(port_IP==0){
			       			printf("Error because port and IP not  specified\n");
			       		}
			       		else if(count == 2){											// Download a single file
			       			int ret;
			       			ret = fork();
			       			if(ret == -1){
			       				printf("Error in creating child process\n");
			       				exit(1);
			       			}
			       			else if(ret == 0){
			       				execl("get-one-file-sig","get-one-file-sig",tokens[1],IP,port,"display",NULL);
			       				// exit(0);
			       			}
			       			else{
			       				int status;
			       				waitpid(ret, &status, 0);
			       				if(status==0){
			       					printf("Child process over\n");
			       				}
			       				else{
			       					printf("Some problem\n");
			       				}
			       			}
			       		}
			       		else if(count == 4 && strcmp(tokens[2], ">")==0){				// Redirect the output to a file

			       			int ret;
			       			ret = fork();
			       			if(ret == -1){
			       				printf("Error in creating child process\n");			
			       				exit(1);
			       			}
			       			else if(ret == 0){
			       				int fd1 = creat(tokens[3] , 0644) ;						 // Creating a new fd for the output file
			       				if(fd1 == -1){
			       					printf("Some error occurred\n");
			       					exit(1);
			       				}
	      						dup2(fd1, STDOUT_FILENO);								// Copying it onto the stdout of the child
	        					close(fd1);
	        					execl("get-one-file-sig"," ",tokens[1],IP,port,"display",NULL);
	        					// exit(0);
			       			}
			       			else{
			       				int status;
			       				waitpid(ret, &status, 0);
			       				if(status==0){
			       					printf("Child process over\n");
			       				}
			       				else{
			       					printf("Some problem\n");
			       				}
			       			}				       	
			       		}

			       		else if(strcmp(tokens[2], "|")==0){

			       			//**********************************************
			       				// Code for piping
			       			int fd[2];
			       			int ret;
			       			int ret1;
			       			pipe(fd);
			       			ret = fork();
			       			if(ret == -1){
			       				printf("Error in creating child process\n");
			       				exit(1);
			       			}
			       			else if(ret == 0){

			       				dup2(fd[1], STDOUT_FILENO);								// duplicate the write end
			       				close(fd[0]);											// Close the open file descriptors which are not needed
			       				close(fd[1]);
			       				execl("get-one-file-sig", " ", tokens[1], IP, port, "display",NULL);
			       			}
			       			else{
			       				ret1 = fork();
			       				if(ret1 == -1){
			       					printf("Error in creating child process\n");
			       					exit(1);
			       				}
			       				if(ret1 == 0){
			       					dup2(fd[0], STDIN_FILENO);							// Copying the read end
			       					close(fd[0]);
			       					close(fd[1]);
			       					char **t = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
			       					int i1 = 0;
			       					for (i1 = 3; i1 < count; ++i1)
			       					{
			       						t[i1-3]=tokens[i1];
			       					}
			       					t[i1-3]=NULL;										// Separating the command after the pipe
			       					execvp(t[0],t);
			       					free(t);
			       				}
			       				else{
				       				int status;
				       				int status1;
				       				close(fd[0]);
				       				close(fd[1]);
				       				waitpid(ret, &status, 0);
				       				waitpid(ret1, &status1, 0);
				       				if(status==0){
				       					printf("Child process over\n");
				       				}
				       				else{
				       					printf("Some problem\n");
				       				}
				       				if(status1==0){
				       					printf("Child process over\n");
				       				}
				       				else{
				       					printf("Some problem\n");
				       				}
			       				}
			       			}
			       			//**********************************************
			       		}

			       		else{
			       			printf("Error in the arguments provided\n");
			       		}

			       }
			       else if(strcmp(tokens[0],"getsq")==0){
			       	if(count==1 || port_IP==0){
			       		printf("Error in number of arguments or server port not initialised\n");
			       	}
			       	else{
			       		int i1 = 0;
			       		if(sq == 1){
			       			sq = 0;
			       		}
			       		for (i1 = 0; i1 < count-1; ++i1)
			       		{
			       			if(sq == 1){												// To stop if given SIGINT
			       				sq = 0;
			       				break;
			       			}
			       			int ret = fork();
			       			if(ret==-1){
			       				printf("Error in creating child process\n");
			       				exit(1);
			       			}
			       			else if(ret == 0){
			       				execl("get-one-file-sig"," ",tokens[i1+1],IP,port,"nodisplay",NULL);
			       				// exit(0);
			       			}
			       			else{
			       				int status;
			       				waitpid(ret, &status, 0);
			       				if(status==0){
			       					printf("Child process over\n");
			       				}
			       				else{
			       					printf("Some problem\n");
			       				}
			       			}
			       		}
			       	}
			       }
			       else if(strcmp(tokens[0],"getpl")==0){
			       		if(count==1 || port_IP==0){
			       			printf("Error in number of arguments or server port not initialised\n");
			       		}
			       		else{
			       			//**********************************************
			       				// Code for parallel
			       			int * pidarray;
			       			int j = 0;
			       			pidarray =(int *)malloc(sizeof(int)*MAX_INPUT_SIZE);
			       			for (j = 0; j < count+1; ++j)
			       			{
			       				pidarray[j]=0;
			       			}
			       			int index = 0;

			       			for(index = 0; index < count-1; index++){
			       				pidarray[index]= fork();
			       				if(pidarray[index]==-1){
			       					printf("Error in creating child process\n");
			       					exit(1);
			       				}
			       				if(pidarray[index] == 0){
			       					execl("get-one-file-sig"," ",tokens[index+1],IP,port,"nodisplay",NULL);
			       					// exit(0);
			       				}
			       			}
			       			int i1 = 0;
			       			for (i1 = 0; i1 < count-1; ++i1)
			       			{
			       				int status;
			       				waitpid(pidarray[i1], &status, 0);
			       				if(status==0){
			       					printf("Child process over\n");
			       				}
			       				else{
			       					printf("Some problem\n");
			       				}
			       			}
			       			free(pidarray);

			       			//**********************************************
			       		}
			       }
			       else if(strcmp(tokens[0],"getbg")==0){
			       		if(count != 2 || port_IP==0){
			       			printf("Error in number of arguments or not initialised\n");
			       		}
			       		else{
			       			int ret;
			       			ret = fork();
			       			if(ret==-1){
			       				printf("Error in creating child process\n");
			       				exit(1);
			       			}		
			       			else if(ret == 0){
			       				execl("get-one-file-sig"," ",tokens[1],IP,port,"nodisplay",NULL);
			       				exit(0);
			       			}
			       			else{
			       				//*****************************************
			       				//Code for background
			       				setpgid(ret,0);
			       				pid_array_background[countpid_background++]=ret;
			       				//*****************************************
			       			}
			       		}
			       }
			       else if(strcmp(tokens[0],"ps")==0||strcmp(tokens[0],"ls")==0||strcmp(tokens[0],"diff")==0||strcmp(tokens[0],"clear")==0||strcmp(tokens[0],"cat")==0||strcmp(tokens[0],"echo")==0||strcmp(tokens[0],"mv")==0||strcmp(tokens[0],"mkdir")==0||strcmp(tokens[0],"cp")==0||strcmp(tokens[0],"find")==0||strcmp(tokens[0],"rm")==0||strcmp(tokens[0],"grep")==0)
			       {
			            int ret;
			            ret = fork();
			            if(ret == -1)
			            {
			                printf("Error creating new process\n");
			                exit(1);
			            }
			            else if(ret==0)
			            {
			                execvp(tokens[0],tokens);
			                // exit(0);
			            }
			            else
			            {
			                int status;
			                waitpid(ret,&status,0);
			                if(status==0)
			                    printf("Child process over\n");
			                else
			                    printf("Some problem\n");
			            }
			        }
			        else{
			        	printf("Wrong Input!!\n");
			        }


			       // Freeing the allocated memory	
			       for(i=0;tokens[i]!=NULL;i++){
				       free(tokens[i]);
			       }
			       free(tokens);
			     }

       }
       
     

}

               