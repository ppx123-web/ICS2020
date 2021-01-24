#include <cpu/exec.h>
#include "local-include/rtl.h"

typedef struct CS_REG
{
  uint32_t RPL : 2;
  uint32_t TI : 1;
  uint32_t idx : 29;
} CS;

#define RPL(cs) ((cs)&0x3)

void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr)
{
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  //TODO();
  
  uint32_t cs = cpu.cs;
  if (RPL(cs) == 0)
  {
    rtl_push(s, &cpu.eflags.eflags_value);
    cpu.eflags.IF = 0;
    cpu.eflags.TF = 0;
    rtl_push(s, &cpu.cs);
    rtl_push(s, &ret_addr);
    uint32_t idt_addr = cpu.idtr.base + 8 * NO;
    uint32_t jmp_pc = vaddr_read(idt_addr, 2) | (vaddr_read(idt_addr + 4, 4) & 0xffff0000);
    rtl_j(s, jmp_pc);
  }
  else if(RPL(cs) == 3) {
    uint32_t gdt_idx = cpu.TR;
    uint32_t gdt_addr = cpu.gdtr.base + gdt_idx;
    uint32_t base = vaddr_read(gdt_addr+2,2) 
                  | (vaddr_read(gdt_addr+4,1)<<16) 
                  | (vaddr_read(gdt_addr+7,1)<<24);
    uint32_t ksp = vaddr_read(base+4,4);
    uint32_t prev = cpu.esp;
    if(ksp !=0)
      cpu.esp = ksp;
    rtl_push(s,&cpu.ss);
    rtl_push(s,&prev);
    rtl_push(s, &cpu.eflags.eflags_value);
    cpu.eflags.IF = 0;
    cpu.eflags.TF = 0;
    rtl_push(s, &cpu.cs);
    rtl_push(s, &ret_addr);
    uint32_t idt_addr = cpu.idtr.base + 8 * NO;
    uint32_t jmp_pc = vaddr_read(idt_addr, 2) | (vaddr_read(idt_addr + 4, 4) & 0xffff0000);
    rtl_j(s, jmp_pc);
    vaddr_write(base+4,0,4);
    //only use ss0 and esp0, esp0 <--->ksp   
  } else {
    printf("wrong RPL\n");
    assert(0);
  }
}

void query_intr(DecodeExecState *s)
{
  assert(cpu.INTR == 0 || cpu.INTR == 1);
  if (cpu.INTR == 1 && cpu.eflags.IF == 1)
  {
    assert(cpu.INTR == 1);
    cpu.INTR = false;
    raise_intr(s, 32, cpu.pc);
    update_pc(s);
  }
}
