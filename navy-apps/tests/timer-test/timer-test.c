#include <stdio.h>
#include <sys/time.h>
#include <assert.h>
#include <NDL.h>

int main() {
  struct timeval num; 
  num.tv_sec = 0;
  printf("time-test starts begin = %d\n",num.tv_sec);
  int sec = 1;
  while(1)
  {
    while(num.tv_sec <= sec)
    {
      int t = gettimeofday(&num,NULL);
      
      assert(t==0);
    }
    
    printf("time: seconds = %d   sec=%d usec%d\n",sec,num.tv_sec,num.tv_usec);
    sec++;
  } 
}

/* int t=0;
  printf("time-test starts\n");
  int sec = 1;
  //NDL_Init(1);
  while(1)
  {
    while(t <= sec)
    {
      t = NDL_GetTicks()/1000000;
    }
      printf("?\n");

    printf("time: seconds = %d\n",sec);
    sec++;
  } 
  NDL_Quit(); */
