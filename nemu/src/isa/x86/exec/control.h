#include "cc.h"

static inline def_EHelper(jmp) {
  // the target address is calculated at the decode stage
  //rtl_j(s, s->jmp_pc);
  if(s->isa.is_operand_size_16)
  {
    rtl_andi(s,&s->jmp_pc,&s->jmp_pc,0xffff);
  }
  rtl_j(s,s->jmp_pc);
  print_asm("jmp %x", s->jmp_pc);
}

static inline def_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint32_t cc = s->opcode & 0xf;
  rtl_setcc(s, s0, cc);
  rtl_jrelop(s, RELOP_NE, s0, rz, s->jmp_pc);

  print_asm("j%s %x", get_cc_name(cc), s->jmp_pc);
}

static inline def_EHelper(jmp_rm) {
  if(s->isa.is_operand_size_16)
  {
    rtl_andi(s,ddest,ddest,0xffff);
    rtl_jr(s,ddest);
  }
  else{
    rtl_jr(s,ddest);
  }

  print_asm("jmp *%s", id_dest->str);
}

static inline def_EHelper(call) {
  // the target address is calculated at the decode stage
  if(s->isa.is_operand_size_16)
  {
    rtl_push(s,&s->seq_pc);
    rtl_andi(s,&s->jmp_pc,&s->jmp_pc,0xffff);
  }
  else{
    rtl_push(s,&s->seq_pc);
  }
  rtl_j(s,s->jmp_pc);
  print_asm("call %x", s->jmp_pc);
}

static inline def_EHelper(ret) {
  rtl_pop(s,s0);
  s->jmp_pc = *s0;
  rtl_j(s,s->jmp_pc);
  print_asm("ret");
}

static inline def_EHelper(ret_imm) {
  TODO();

  print_asm("ret %s", id_dest->str);
}

static inline def_EHelper(call_rm) {
  if(s->isa.is_operand_size_16)
  {
    rtl_push(s,&s->seq_pc);
    rtl_andi(s,&s->seq_pc,ddest,0xffff);
  }
  else{
    rtl_push(s,&s->seq_pc);
    rtl_jr(s,ddest);
    //s->seq_pc = *ddest;
  }

  print_asm("call *%s", id_dest->str);
}

/*
t0, t1, ... - 只能在RTL伪指令的实现过程中存放中间结果
s0, s1, ... - 只能在译码辅助函数和执行辅助函数的实现过程中存放中间结果
*/