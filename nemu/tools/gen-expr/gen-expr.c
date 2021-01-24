#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>


// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";


#define len 20000

int idx;

uint32_t choose(uint32_t x)
{
  return rand()%x;
}

void gen(int x)
{
  buf[idx++]=x;
}

void gen_num()
{
  char temp[15];
  sprintf(temp,"%d",rand());
  int length=strlen(temp);
  for(int i = 0;i < length;i++)
  {
    buf[idx++]=temp[i];
  }
  if(length!=0)
    buf[idx++]='U';
}

void gen_rand_op(void)
{
  switch(choose(4))
  {
    case 0: buf[idx++]='+';break;
    case 1: buf[idx++]='-';break;
    case 2: buf[idx++]='*';break;
    case 3: buf[idx++]='/';break;
  }
}

void gra(void)
{
  int temp = choose(5);
  for(int i = 0;i < temp;i++)
  {
    buf[idx++]=' ';
  }
}

static inline void gen_rand_expr() {
  if(idx > len)
    {
      gen_num();
      return;
    }
  gra();
  switch (choose(3)) {
    case 0: 
      gen_num();
      break;
    case 1: 
      gen('('); 
      gen_rand_expr(); 
      gen(')');
      break;
    default: 
      gen_rand_expr(); 
      gen_rand_op();  
      gen_rand_expr(); 
      break;
  }
    
}


int main(int argc, char *argv[]) {
  unsigned seed = time(NULL);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    buf[0]='\0';
    code_buf[0]='\0';
    idx=0;
    gen_rand_expr();
    buf[idx]='\0';
    

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc -Werror /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result=-1;
    fscanf(fp, "%u", &result);
    pclose(fp);

    for(int i = 0;i < strlen(buf);i++)
    {
      if(buf[i]=='U')
        buf[i]=' ';
    }
    if(result!=-1)
      printf("%u %s\n", result, buf);
    else i--;


  }
  return 0;
}
