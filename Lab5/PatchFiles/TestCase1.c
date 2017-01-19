#include "types.h"
#include "user.h"
#include "stat.h"

int main(int argc, char *argv[])
{
	int ret;
	int Priority = 100;
	// Set parent's priority to a very high value
	setprio(Priority);
	printf(1,"Made Child 1\n");
	ret = fork();
	if(ret==0)
	{
		Priority = 6;
		int start = uptime();
		setprio(Priority);
		printf(1,"Process 1 Started with Priority %d\n",getprio());
		sleep(2);
		int count=0;
		printf(1,"Switched in 1\n");
		while(1){count++;}
		printf(1,"Exited Child 1 %d\n",uptime()-start);
		exit();
	}
	else
	{
		printf(1,"Made Child 2\n");
		ret = fork();
		if(ret==0)
		{
			Priority = 8;
			int start = uptime();
			setprio(Priority);
			printf(1,"Process 2 Started with Priority %d\n",getprio());
			sleep(2);
			printf(1,"Switched in 2\n");
			int count=0;
			while(1){count++;}
			printf(1,"Exited Child 2 %d\n",uptime()-start); 
			exit();
		}
		else
		{
			printf(1,"Made Child 3\n");
			ret=fork();
			if(ret==0)
			{
				int start = uptime();
				Priority=10;
				setprio(Priority);
				printf(1,"Process 3 Started with Priority %d\n",getprio());
				sleep(2);
				int count=0;
				printf(1,"Switched in 3\n");
				while(1){count++;}
				printf(1,"Exited Child 3 %d\n",uptime()-start);
				exit();
			}
			else
			{
				int count=3;
				while(count>0)
				{
					wait();
					count--;
				}
				exit();
			}
		}		
	}

	return 0;
}