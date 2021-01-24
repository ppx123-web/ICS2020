#include <memory.h>
#include <proc.h>

#define OFFSET 0xfff
#define PGSIZE 4096
#define page_begin_vaddr(A) ((void*)((uintptr_t)(A)&(~OFFSET)))
#define page_end_vaddr(A) ((void*)(((uintptr_t)(A)&(~OFFSET))+PGSIZE))
#define min(A,B) ((size_t)(A) < (size_t)(B)? (A):(B))
#define vaddr_offset(A) ((uintptr_t)(A)&OFFSET)

static void *pf = NULL;
void map(AddrSpace *as, void *va, void *pa, int prot);
extern PCB *current;

void *new_page(size_t nr_page)
{
  void *p = pf;
  pf = pf + nr_page * PGSIZE;
  return p;
  //return NULL;
}

static inline void *pg_alloc(int n)
{
  void *ret = new_page(n / PGSIZE);
  memset(ret, 0, n);
  return ret;
  //return NULL;
}

void free_page(void *p)
{
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk)
{
  if (current->max_brk >= brk)
  {
    return 0;
  }
  else
  {
    void *vaddr = (void *)current->max_brk;
    void *end = (void *)brk;
    void *paddr = NULL;
    uintptr_t len = (uintptr_t)min(page_end_vaddr(vaddr) - vaddr, end - vaddr);
    if(len < PGSIZE) {
      vaddr += len;
    } else if(len == PGSIZE) {
      assert(vaddr_offset(vaddr) == 0);
    } else assert(0);
    while((uintptr_t)vaddr < (uintptr_t)end)
    {
      len = (uintptr_t)min(page_end_vaddr(vaddr) - vaddr, end - vaddr);
      paddr = new_page(1);
      map(&current->as,page_begin_vaddr(vaddr),paddr,0);
      vaddr += len;
    }
    current->max_brk = brk;
  }
  return 0; 
}

void init_mm()
{
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);
#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
