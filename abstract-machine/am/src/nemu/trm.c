#include <am.h>
#include <nemu.h>

extern char _heap_start;
int main(const char *args);

Area heap = RANGE(&_heap_start, PMEM_END);         //堆区的起始和末尾
#ifndef MAINARGS
#define MAINARGS ""
#endif
static const char mainargs[] = MAINARGS;

void putch(char ch) {
  outb(SERIAL_PORT, ch);
}//输出一个字符

void halt(int code) {
  nemu_trap(code);

  // should not reach here
  while (1);
}//用于结束程序的运行

void _trm_init() {
  int ret = main(mainargs);
  halt(ret);
}//初始化
