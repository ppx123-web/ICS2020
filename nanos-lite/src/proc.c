#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;//decide which process to use now;

void naive_uload(PCB *pcb, const char *filename);
Context* kcontext(Area kstack, void (*entry)(void *), void *arg);
void context_kload(PCB* create_pcb, void (*entry)(void *), void *arg);
void context_uload(PCB *pcb,const char* filename,char *const argv[], char* const envp[]);
//void context_uload(PCB *pcb,const char* filename);

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    //printf("1\n");
    //printf("%d addr %p\n",(uintptr_t)arg,&arg);
    //if(j%100000 == 0)
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    //while(j > 2);
    yield();
  }
}

static int fg_pcb = 1;
void set_state(int state) {
  fg_pcb = state;
}


void init_proc() {
  //context_kload(&pcb[0], hello_fun, (void*)22);
  //context_kload(&pcb[0], hello_fun, (void*)5);
  char * argv[] = {"/bin/exec-test","/bin/exec-test","--skip",NULL},*envp[]={NULL};
  char * empty [] ={NULL};
  context_uload(&pcb[0] ,"/bin/hello",empty,empty);
  context_uload(&pcb[1], "/bin/pal",argv,envp);
  //context_uload(&pcb[1], "/bin/pal",empty,empty);
  context_uload(&pcb[2], "/bin/bird",empty,empty);
  context_uload(&pcb[3], "/bin/nslider",empty,empty);

  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here

}


static int cnt = 1;

Context* schedule(Context *prev) {

  current->cp = prev;
  //current = &pcb[1];
  if(cnt % 0x64 == 0) {
    cnt = 1;
    current = &pcb[0];
  }
  else {
    cnt++;
    current = &pcb[fg_pcb];
  }
  //current = current == &pcb[0]? &pcb[fg_pcb]:&pcb[0];
  return current->cp;
}//效果是hello出现是每次出二到三行，原因在于时钟中断每次切换的时间只够运行输出2-3行的内容。

void context_kload(PCB* create_pcb, void (*entry)(void *), void *arg)
{
  Area stack = {create_pcb->stack,create_pcb->stack + STACK_SIZE};
  //assert((void*)create_pcb->stack == create_pcb->as.area.start);
  create_pcb->cp = kcontext(stack,entry,arg);
  //printf("%x  %x\n",create_pcb->cp->esp,create_pcb->cp->ebp);
}//注意变量名，创建上下文要在进程的栈的末尾

