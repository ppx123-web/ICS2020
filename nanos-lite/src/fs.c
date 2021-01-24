#include <fs.h>
#include <proc.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);
#define file_num sizeof(file_table)/sizeof(Finfo)

void naive_uload(PCB *pcb, const char *filename);
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);
size_t fb_sync(const void *buf,size_t offset,size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB,FD_EVENTS,FD_DISPINFO,FD_SYNC};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_FB] = {"/dev/fb",0,0,invalid_read,fb_write},
  [FD_EVENTS] = {"/dev/events",0,0,events_read,invalid_write},
  [FD_DISPINFO] = {"/proc/dispinfo",64,0,dispinfo_read,invalid_write},
  [FD_SYNC] = {"/dev/sync",0,0,invalid_read,fb_sync},
#include "files.h"
};


//排好序,和列表中不一样会有些不在表中！！！

void init_fs() {
  //TODO: initialize the size of /dev/fb
  int w = io_read(AM_GPU_CONFIG).width;
  int h = io_read(AM_GPU_CONFIG).height;
  file_table[FD_FB].size = w * h * 4;
  assert(file_table[FD_SYNC].write != NULL);
}

int fs_open(const char *pathname, int flags, int mode)
{
  size_t id=0;
  int i = 0;
  for(i = 0;i < file_num;i ++)
  {
    if(strcmp(file_table[i].name,pathname)==0)
    {
      id=i;
      file_table[i].open_offset = 0;
      break;
    }
  }
  if(i == file_num) {
    return -1;
  }
  assert(i != file_num);
  return id;
}

size_t fs_read(int fd, void *buf, size_t len)
{
  Finfo* f = &file_table[fd];
  if(f->read == NULL)
  {
    if(f->open_offset >= f->size)
    {
      assert(f->open_offset <= f->size);
      //printf("%s open_offset = %d size = %d FULL\n",f->name,f->open_offset,f->size);
      return 0;
    }
    else {
      size_t l = len <= file_table[fd].size - file_table[fd].open_offset? len:file_table[fd].size - file_table[fd].open_offset;
      ramdisk_read(buf,file_table[fd].disk_offset+file_table[fd].open_offset,l);
      file_table[fd].open_offset = file_table[fd].open_offset+l;
      assert(file_table[fd].open_offset <= file_table[fd].size);
      return l;
    }
  }
  else{
    int ret = f->read(buf,file_table[fd].open_offset,len);
    f->open_offset+=len;
    //size_t l = len <= file_table[fd].size - file_table[fd].open_offset? len:file_table[fd].size - file_table[fd].open_offset;
    //file_table[fd].open_offset = file_table[fd].open_offset+l;
    return ret;
  }//以后可能出现问题
  
}//remained to be thinking about fd == 0,1,2, .e.t stdin,stdout,stderror


size_t fs_write(int fd, const void *buf, size_t len)
{
  Finfo* f = &file_table[fd];
  if(f->write!=NULL) 
  {
    int l = f->write(buf,f->open_offset,len);
    f->open_offset += l;
    return l;
  }
  else{
    int ret = file_table[fd].size - file_table[fd].open_offset <= len? file_table[fd].size - file_table[fd].open_offset:len;
    ramdisk_write(buf,file_table[fd].disk_offset+file_table[fd].open_offset,ret);
    file_table[fd].open_offset +=ret;
    assert(file_table[fd].open_offset <= file_table[fd].size);
    //printf("write %s\n",f->name);
    return ret;
  }
}

size_t fs_lseek(int fd, size_t offset, int whence)
{
  //printf("%d\n",offset);
  Finfo* f = &file_table[fd];
  size_t ret=-1;
  switch(whence){
    case SEEK_SET:{
      if(0<=offset && offset<=f->size){
        f->open_offset = offset;
        ret=f->open_offset;
      }
      break;
    }
    case SEEK_CUR:{
      if((offset+f->open_offset>=0)&&(offset+f->open_offset<=f->size)){
        f->open_offset+=offset;
        ret = f->open_offset;
      }
      break;
    }
    case SEEK_END:{
      if((offset+f->size>=0)&&(offset+f->size<=f->size)){
        f->open_offset = f->size+offset;
        ret = f->open_offset;
      }
      break;
    }
  }
  //printf("lseek = %d size = %d\n",file_table[fd].open_offset,file_table[fd].size);
  //printf("%s\n",file_table[fd].name);
  //assert(file_table[fd].open_offset <= file_table[fd].size);
  return ret;
}

int fs_close(int fd)
{
  return 0;
}
