#include "types.h"
#include "user.h"
#include "stat.h"

int main(int argc, char const *argv[])
{
  int x = getNumFreePages();
  printf(1, "%d\n", x);
  int ret = fork();
  if(ret == 0){
    int y = getNumFreePages();
    printf(1, "%d\n", y);
    exit();
  }
  else{
    wait();
    x = getNumFreePages();
    printf(1, "%d\n", x);
  }
  exit();
}