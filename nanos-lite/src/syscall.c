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
int sys_execve(char* filename,char * const argc[],char* const envp[]);
//intptr_t sys_sbrk(void* addr);

/* extern char _end;
static void* program_break = &_end; */
/* extern char end;
void *prog_break=(void *)(&end); */

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  //printf("do_syscall a[0] = %d  a[1] = %d a[2] = %x  a[3] = %d\n",a[0],a[1],a[2],a[3]);
  switch (a[0]) {
    case SYS_exit: 
      naive_uload(NULL,"/bin/nterm");
      halt(a[1]); break;
    case SYS_yield: yield(); c->GPRx = 0;break;
    case SYS_write: c->GPRx = fs_write(a[1],(void*)a[2],a[3]); break;
    case SYS_brk: c->GPRx = 0;break;
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

int sys_execve(char* filename,char * const argc[],char* const envp[])
{
  int ret = 0;
  for(int cnt = 0;envp[cnt] != NULL;cnt+=1)
  {
    char* path = (char*)malloc(strlen(filename)+strlen(envp[cnt])+5);
    strcpy(path,envp[cnt]);
    memset(path+strlen(envp[cnt])/2+1,0,strlen(envp[cnt]));
    path = strcat(path,"/");
    path = strcat(path,filename);
    //printf("%s\n",path);
    if(fs_open(path,0,0)!= -1)
    {
      //printf("%s\n",path);
      naive_uload(NULL,path);
      break;
    }
  }
  ret = fs_open(filename,0,0);
  return ret;
}

