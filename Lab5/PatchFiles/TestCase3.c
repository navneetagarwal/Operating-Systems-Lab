#include "types.h"
#include "user.h"
#include "stat.h"
#include "fcntl.h"
//#include <sys/wait.h>

int main(int argc, char *argv[])
{
  int ret;
  int i;
  int Priority = 100;
  // Set parent's priority to a very high value
  setprio(Priority);
  printf(1,"Made Child 1\n");
  ret = fork();
  if(ret==0)
  {
    int start = uptime();
    Priority = 6;
    setprio(Priority);
    printf(1,"Process 1 Started with Priority %d\n",getprio());
    sleep(4);
    printf(1,"Switched in 1\n");
    int count=0;
    char * buff = "adaddsa";
    int fd = open("xy1", O_RDWR);
    for(i=0;i<10000000;i++)
    {
      write(fd,buff,5);
      count++;
    }
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
      sleep(4);
      printf(1,"Switched in 2\n");
      int count=0;
      char * buff = "adaddsa";
      int fd = open("xy2", O_RDWR);
      for(i=0;i<10000000;i++)
      {
        write(fd,buff,5);
        count++;
      }
      printf(1,"Exited Child 2 %d\n",uptime()-start); 
      exit();
    }
    else
    {
      printf(1,"Made Child 3\n");
      ret=fork();
      if(ret==0)
      {
        Priority=10;
        int start = uptime();
        setprio(Priority);
        printf(1,"Process 3 Started with Priority %d\n",getprio());
        sleep(4);
        int count=0;
        printf(1,"Switched in 3\n") ;
        char * buff = "adaddsa";
        int fd = open("xy3", O_RDWR);
        for(i=0;i<10000000;i++)
        {
          write(fd,buff,5);
          count++;
        }
        printf(1,"Exited Child 3 %d\n",uptime()-start);
        exit();
      }
      else
      {
        printf(1,"Made Child 4\n");
        ret = fork();
        if(ret == 0){
          int start = uptime();
          Priority = 12;
          setprio(Priority);
          printf(1,"Process 4 Started with Priority %d\n",getprio());
          sleep(4);
          int count = 0;
          printf(1,"Switched in 4\n") ;
          char * buff = "adaddsa";
          int fd = open("xy4", O_RDWR);
          for(i=0;i<10000000;i++)
          {
            write(fd,buff,5);
            count++;
          } 
          printf(1,"Exited Child 4 %d\n",uptime()-start);
          exit();

        }
        else{
          int count=4;
          while(count>0)
          {
            wait();
            count--;
          }
          exit();
        }
      }
    }   
  }

  return 0;
}