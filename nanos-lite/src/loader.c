#include <proc.h>
#include <elf.h>
#include <fs.h>
#include <memory.h>

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif

#define PGSIZE 4096
#define stack_num 8
#define OFFSET 0xfff
#define page_begin_vaddr(A) ((void*)((uintptr_t)(A)&(~OFFSET)))
#define page_end_vaddr(A) ((void*)(((uintptr_t)(A)&(~OFFSET))+PGSIZE))
#define paddr_offset(A,B) ((void*)(((uintptr_t)(A)&(OFFSET))|(uintptr_t)(((uintptr_t)(B)>>12)<<12)))
#define min(A,B) ((uintptr_t)(A) < (uintptr_t)(B)? (A):(B))
void* pt_walk(uintptr_t vaddr,PCB* pcb);


size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();
int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

Context *ucontext(AddrSpace *as, Area kstack, void *entry);
void switch_boot_pcb();
void protect(AddrSpace *as);
void *paddr_page(size_t nr_page);
void map(AddrSpace *as, void *va, void *pa, int prot);

static uintptr_t loader(PCB *pcb, const char *filename)
{
  Elf_Ehdr elfhdr;
  Elf_Phdr prohdr;
  size_t fd = fs_open(filename, 0, 0);
  fs_read(fd, &elfhdr, sizeof(Elf_Ehdr));
  assert(fd != -1);
  for (int i = 0; i < elfhdr.e_phnum; i++)
  {
    fs_lseek(fd, elfhdr.e_phoff + i * sizeof(Elf_Phdr), SEEK_SET);
    fs_read(fd, &prohdr, sizeof(Elf_Phdr));
    void *vaddr = (void *)(prohdr.p_vaddr);
    void *paddr = NULL;
    void *file_vaddr = (void*)(prohdr.p_vaddr + prohdr.p_filesz);
    void *mem_vaddr = (void*)(prohdr.p_vaddr + prohdr.p_memsz);
    size_t read_len = 0;
    if (prohdr.p_type == PT_LOAD)
    {
      fs_lseek(fd, prohdr.p_offset, SEEK_SET);
      while((size_t)vaddr < (size_t)file_vaddr) 
      {
        paddr = new_page(1);
        read_len = (size_t)min((page_end_vaddr(vaddr) - vaddr),file_vaddr - vaddr);
        map(&pcb->as,page_begin_vaddr(vaddr),paddr,0x7);
        fs_read(fd, paddr_offset(vaddr,paddr),read_len);
        vaddr += read_len;
      }
      assert(vaddr == file_vaddr);

      while((size_t)vaddr < (size_t)mem_vaddr)
      {
        if(((uintptr_t)vaddr&OFFSET)==0) {
          paddr = new_page(1);
          map(&pcb->as,page_begin_vaddr(vaddr),paddr,0x7);
        }
        read_len = (size_t)min((page_end_vaddr(vaddr) - vaddr),mem_vaddr - vaddr);
        memset(paddr_offset(vaddr,paddr),0,read_len);
        vaddr += read_len;
      }
      assert(vaddr == mem_vaddr);
      pcb->max_brk = (uintptr_t)vaddr;
    } 
  }
  fd = fs_close(fd);
  return elfhdr.e_entry;

  /*
  获取程序的大小之后, 以页为单位进行加载: 
  申请一页空闲的物理页
  通过map()把这一物理页映射到用户进程的虚拟地址空间中
  从文件中读入一页的内容到这一物理页中
  */
}
//静态变量的初始化问题，好像是在programmmer header中加载的，

void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[])
{
  protect(&pcb->as);
  void *start = new_page(stack_num);
  void *end = start + stack_num * PGSIZE;
  for (int i = 1; i <= stack_num; i++)
  {
    map(&pcb->as, pcb->as.area.end - i * PGSIZE, end - i * PGSIZE, 0x7);
  }
  Area ustack = {start, end};

  uintptr_t entry = loader(pcb, filename);

  int size = 0, size_argv = 0, size_envp = 0, argc = 0, envc = 0;
  while (argv[argc] != NULL)
  {
    size_argv += strlen(argv[argc]) + 1;
    argc++;
  }
  while (envp[envc] != NULL)
  {
    size_envp += strlen(envp[envc]) + 1;
    envc++;
  }
  size = size_envp + size_argv + sizeof(uintptr_t) * (argc + 4 + envc);
  size = size - size % sizeof(uintptr_t);
  void *ret = pcb->as.area.end - size;
  void *args_start = ustack.end - size;
  void *str_start = args_start + sizeof(uintptr_t) * (argc + 3 + envc);

  memset(args_start, 0, ustack.end - args_start);
  *(uintptr_t *)args_start = argc;
  for (int i = 0; i < argc; i++)
  {
    memcpy(str_start, argv[i], strlen(argv[i]));
    *(uintptr_t *)(args_start + sizeof(uintptr_t) * (i + 1)) = (uintptr_t)str_start;
    str_start += strlen(argv[i]) + 1;
  }
  *(uintptr_t *)(args_start + sizeof(uintptr_t) * (1 + argc)) = 0;
  for (int i = 0; i < envc; i++)
  {
    memcpy(str_start, envp[i], strlen(envp[i]));
    *(uintptr_t *)(args_start + sizeof(uintptr_t) * (argc + 2 + i)) = (uintptr_t)str_start;
    str_start += strlen(envp[i]) + 1;
  }
  *(uintptr_t *)(args_start + sizeof(uintptr_t) * (argc + 2 + envc)) = 0;

  Area stack = {pcb->stack, pcb->stack + STACK_SIZE};
  pcb->cp = ucontext(&pcb->as, stack, (void *)entry);
  pcb->cp->GPRx = (uintptr_t)ret;
  pcb->cp->esp3 = (uintptr_t)ret;
  /* uintptr_t entry = loader(pcb, filename);
  Area stack = {pcb->stack,pcb->stack + STACK_SIZE};
  pcb->cp = ucontext(&pcb->as,stack,(void*)entry);
  pcb->cp->GPRx = (uintptr_t)heap.end; */
  //while (1);
}



void* pt_walk(uintptr_t vaddr,PCB* pcb)
{
  uintptr_t va_dir_idx = (vaddr >> 22) & 0x3ff;
  uintptr_t va_page_table_idx = (vaddr >> 12) & 0x3ff;
  uintptr_t* page_dir_base = (uintptr_t*)pcb->as.ptr;
  uintptr_t page_table_entry = page_dir_base[va_dir_idx];
  if((page_table_entry & 0x1) == 0) return (void*)(-1);
  assert((page_table_entry & 0x1) == 0x1);
  uintptr_t* page_table_base = (uintptr_t*)page_begin_vaddr(page_table_entry);
  uintptr_t page_table_value = page_table_base[va_page_table_idx];
  if((page_table_value & 0x1) == 0) return (void*)(-1);
  assert((page_table_value & 0x1) == 0x1);
  return page_begin_vaddr(page_table_value);
}