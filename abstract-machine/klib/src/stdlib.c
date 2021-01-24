#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;
static char *addr = NULL;


int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

int abs(int x) {
  return (x < 0 ? -x : x);
}

int atoi(const char* nptr) {
  int x = 0;
  while (*nptr == ' ') { nptr ++; }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr ++;
  }
  return x;
}


void *malloc(size_t size) {
  if(addr == NULL)
    addr = (void *)ROUNDUP(heap.start, 8);
  char* ret = addr;
  size = (size_t)ROUNDUP(size, 8);
  addr=addr+size;
  assert((uintptr_t)heap.start <= (uintptr_t)addr && (uintptr_t)addr < (uintptr_t)heap.end);
  for (char *p = (char *)ret; p != (char *)addr; p ++) {
    *p = 0;
  }
  return ret;
}

void free(void *ptr) {
}

#endif
