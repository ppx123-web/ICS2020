#include <stdio.h>
#include <assert.h>

int main() {
  printf("start\n");
  FILE *fp = fopen("/share/files/num", "r+");
  assert(fp);
  printf("opened\n");
  fseek(fp, 0, SEEK_END);
  printf("%d\n",fp);
  long size = ftell(fp);
  assert(size == 5000);
  printf("seek 1\n");	  
	
  fseek(fp, 500 * 5, SEEK_SET);
  printf("seek 2\n");	
  int i, n;
  for (i = 500; i < 1000; i ++) {
    fscanf(fp, "%d", &n);
    //printf("i = %d , n = %d\n",i,n);
    assert(n == i + 1);
  }

  fseek(fp, 0, SEEK_SET);
  printf("seek 3\n");	
  for (i = 0; i < 500; i ++) {
    fprintf(fp, "%4d\n", i + 1 + 1000);
  }

  for (i = 500; i < 1000; i ++) {
    fscanf(fp, "%d", &n);
    //printf("n=%d i=%d\n",n,i);
    assert(n == i + 1);
  }

  fseek(fp, 0, SEEK_SET);
  printf("seek 4\n");	
  for (i = 0; i < 500; i ++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1 + 1000);
  }

  fclose(fp);

  printf("PASS!!!\n");

  return 0;
}
