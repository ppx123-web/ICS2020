#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0xc0000000)

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);//获得第一个页，作为页目录

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_cr3(kas.ptr);
  //printf("kas %x\n",kas.ptr);
  set_cr0(get_cr0() | CR0_PG);
  vme_enable = 1;

  return true;
}
//初始化，接受两个来自操作系统的页面分配回调函数的指针，申请/释放这一页物理页

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);//将最开始的页表复制给内核空间
}
//创建一个默认的地址空间
//分配了虚拟空间地址，

void unprotect(AddrSpace *as) {
}
//销毁指定的地址空间

void __am_get_cur_as(Context *c) {
  c->cr3 = (vme_enable ? (void *)get_cr3() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->cr3 != NULL) {
    set_cr3(c->cr3);
  }
}


/*
+--------10------+-------10-------+---------12----------+
\--- DIR--------/ \--- PAGE------/\------ OFFSET--------/

page table present
pd page directory

in page table 
+-----------20----------+-------11--------+-1-+
\--PAGE FRAME ADDRESS---/\---------------/\-P-/

页目录和页表都有present位，且都是高20位作为索引。
*/

#define PT_P 0x1

void map(AddrSpace *as, void *va, void *pa, int prot) {
  assert(as != NULL);
  //printf("va %x pa %x\n",va,pa);
  uint32_t va_dir_idx = ((uint32_t)va >> 22)&0x3ff;
  uint32_t va_page_table_idx = ((uint32_t)va >> 12)&0x3ff;
  //printf("pdx = %x  ptx = %x\n",va_dir_idx,va_page_table_idx);
  uint32_t page_table_entry = (((uint32_t*)as->ptr)[va_dir_idx]);//得到页目录
  //printf("%p %x %p %x %x\n",as->ptr,va_dir_idx,page_table_entry,va_page_table_idx,va_page_table_idx);
  if((page_table_entry&PT_P) == 0) {
    page_table_entry = (uint32_t)pgalloc_usr(PGSIZE);//低12位都是0
    ((uint32_t*)as->ptr)[va_dir_idx] = (uint32_t)page_table_entry|PT_P;
  }
  assert(((((uint32_t*)as->ptr)[va_dir_idx])&PT_P) == 1);
  uint32_t* page_table = (uint32_t*)((((uint32_t*)as->ptr)[va_dir_idx])&(~0xfff));
  if((page_table[va_page_table_idx]&PT_P) == 0)
  {
    //if(as->ptr != kas.ptr) printf("ptx:%x value = %x vaddr = %p\n",va_page_table_idx,((uint32_t)pa&(~0xfff)),va);
    page_table[va_page_table_idx] = ((uint32_t)pa&(~0xfff)) | PT_P;
  }
  else {
    assert(0);
    page_table[va_page_table_idx] = ((uint32_t)pa&(~0xfff)) | PT_P;
  }
}
//维护映射的API
//将地址空间as中虚拟地址va所在的虚拟页, 以prot的权限映射到pa所在的物理页. 当prot中的present位为0时, 表示让va的映射无效. 


Context* ucontext(AddrSpace *as, Area kstack, void *entry) {

  Context* c = (Context*)(kstack.end) - 1;
  c->eip = (uintptr_t)entry;
  c->cs= USEL(3);
  c->cr3=as->ptr;
  c->eflags = 0x200;
  c->ss3 = USEL(4);
  return c;
  //return NULL;
}//ignore argument as now
