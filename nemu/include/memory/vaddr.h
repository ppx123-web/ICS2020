#ifndef __MEMORY_VADDR_H__
#define __MEMORY_VADDR_H__

#include <common.h>

static inline word_t vaddr_read(vaddr_t addr, int len) {
  word_t vaddr_read1(vaddr_t addr);
  word_t vaddr_read2(vaddr_t addr);
  word_t vaddr_read4(vaddr_t addr);
  switch (len) {
    case 1: return vaddr_read1(addr);
    case 2: return vaddr_read2(addr);
    case 4: return vaddr_read4(addr);
    default: assert(0);
  }
}

static inline void vaddr_write(vaddr_t addr, word_t data, int len) {
  void vaddr_write1(vaddr_t addr, word_t data);
  void vaddr_write2(vaddr_t addr, word_t data);
  void vaddr_write4(vaddr_t addr, word_t data);
  switch (len) {
    case 1: vaddr_write1(addr, data); break;
    case 2: vaddr_write2(addr, data); break;
    case 4: vaddr_write4(addr, data); break;
    default: assert(0);
  }
}

static inline word_t vaddr_ifetch(vaddr_t addr, int len) {
  word_t vaddr_ifetch1(vaddr_t addr);
  word_t vaddr_ifetch2(vaddr_t addr);
  word_t vaddr_ifetch4(vaddr_t addr);
  switch (len) {
    case 1: return vaddr_ifetch1(addr);
    case 2: return vaddr_ifetch2(addr);
    case 4: return vaddr_ifetch4(addr);
    default: assert(0);
  }
}


#define PAGE_SIZE         4096
#define PAGE_MASK         (PAGE_SIZE - 1)
#define PG_ALIGN __attribute((aligned(PAGE_SIZE)))

#endif
