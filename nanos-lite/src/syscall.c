#include <common.h>
#include "syscall.h"
#include <sys/time.h>
#include <proc.h>

void yield();
void halt(int code);
int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);
int sys_gettimeofday(struct timeval *tz,struct timezone *tv);
void naive_uload(PCB *pcb, const char *filename);
int sys_execve(char* filename,char * const argv[],char* const envp[]);
//intptr_t sys_sbrk(void* addr);
void context_uload(PCB *pcb,const char* filename,char *const argv[], char* const envp[]);
void switch_boot_pcb();
int mm_brk(uintptr_t brk);
extern PCB* current;
/* extern char _end;
static void* program_break = &_end; */
/* extern char end;
void *prog_break=(void *)(&end); */

static char *empty_args[] = {NULL};

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  //printf("do_syscall a[0] = %d  a[1] = %d a[2] = %x  a[3] = %d\n",a[0],a[1],a[2],a[3]);
  switch (a[0]) {
    case SYS_exit:
      /*sys_execve("/bin/nterm",NULL,NULL); while(1);break; */
      sys_execve("/bin/nterm",empty_args,empty_args);
      //halt(a[1]);
      break;
    case SYS_yield:yield(); c->GPRx = 0;break;
    case SYS_write: c->GPRx = fs_write(a[1],(void*)a[2],a[3]); break;
    case SYS_brk: c->GPRx = mm_brk(a[1]);break;
    case SYS_open: c->GPRx = fs_open((void*)a[1],a[2],a[3]); break;
    case SYS_read: c->GPRx = fs_read(a[1],(void*)a[2],a[3]); break;
    case SYS_lseek: c->GPRx = fs_lseek(a[1],a[2],a[3]); break;
    case SYS_close: c->GPRx = fs_close(a[1]); break;
    case SYS_gettimeofday: c->GPRx = sys_gettimeofday((struct timeval *)a[1],(struct timezone *)a[2]);break;
        //printf("%d\n",((struct timeval*)a[1])->tv_sec); 
    case SYS_execve: /* naive_uload(NULL,(char*)a[1]); */c->GPRx = sys_execve((char*)a[1],(char**)a[2],(char**)a[3]); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}

int sys_execve(char* filename,char * const argv[],char* const envp[])
{
  int ret = fs_open(filename,0,0);
  //printf("try open:%s\n",filename);
  if(ret == -1) return -2;
  else {
    context_uload(current,filename,argv,envp);
    //printf("%s %s %s\n",filename,argv[0],envp[0]);
    switch_boot_pcb();
    //printf("has loaded\n");
    yield();
    //naive_uload(NULL,filename);
    return 0;
  }
}



