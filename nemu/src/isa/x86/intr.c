#include <cpu/exec.h>
#include "local-include/rtl.h"

void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  //TODO();
  rtl_push(s,&cpu.eflags.eflags_value);
  //printf("%d %d %d\n",cpu.pc,cpu.cs,cpu.eflags.eflags_value);
  cpu.eflags.IF = 0;
  cpu.eflags.TF = 0;
  rtl_push(s,&cpu.cs);
  rtl_push(s,&ret_addr);
  uint32_t idt_addr = cpu.idtr.base + 8*NO;
  uint32_t jmp_pc = vaddr_read(idt_addr,2) | (vaddr_read(idt_addr+4,4)&0xffff0000);
  //printf("%x %x %x %x %x\n",ret_addr,cpu.eax,cpu.ecx,cpu.edx,cpu.ebx);
  //printf("0x%x\n",jmp_pc);
  rtl_j(s,jmp_pc);
}

void query_intr(DecodeExecState *s) {
  TODO();
}
