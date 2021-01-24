#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//#include"monitor/debug/expr.h"

//word_t expr(char *e, bool *success);

void init_monitor(int, char *[]);
void engine_start();
int is_exit_status_bad();

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  init_monitor(argc, argv);

  /*
  FILE * fp=fopen("tools/gen-expr/input","r");
  char expression[70000];
  char *str;
  for(int i = 0;i < 10000;i++)
  {
    bool flag=true;
    
    expression[0]='\0';
    fgets(expression,70000,fp);
    unsigned answer=0,ans;
    str = strtok(expression," ");
    sscanf(expression,"%u",&answer);
    str=expression+strlen(expression)+1;
    ans=expr(str,&flag);
    if(answer!=ans)
      printf("%d Wrong\n",i);
    else printf("correct %d\n",i);
  
  }
  //用于验证答案
  */
  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
