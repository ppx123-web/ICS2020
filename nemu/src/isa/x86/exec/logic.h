#include "cc.h"


static inline def_EHelper(test) {
  rtl_and(s,s0,ddest,dsrc1);
  cpu.eflags.CF = 0;
  cpu.eflags.OF = 0;
  rtl_update_ZFSF(s,s0,s->dest.width);
  /* rtl_msb(s,s1,s0,id_dest->width); */
  /* cpu.eflags.ZF = *s0 == 0;
  cpu.eflags.SF = *s1 == 1; */
  print_asm_template2(test);
}

static inline def_EHelper(and) {
  rtl_and(s,s0,ddest,dsrc1);
  operand_write(s,id_dest,s0);
  rtl_update_ZFSF(s,s0,id_dest->width);
  cpu.eflags.CF=0;
  cpu.eflags.OF=0;
  /* rtl_msb(s,s1,s0,id_dest->width);
  cpu.eflags.ZF = *s0 == 0;
  cpu.eflags.SF = *s1 == 1; */
  print_asm_template2(and);
}

static inline def_EHelper(xor) {
  rtl_xor(s, s0, ddest, dsrc1);
  operand_write(s, id_dest, s0);
  rtl_update_ZFSF(s,s0,id_dest->width);
  cpu.eflags.CF=0;
  cpu.eflags.OF=0;
  /* rtl_msb(s,s1,s0,id_dest->width);
  cpu.eflags.ZF = *s0 == 0;
  cpu.eflags.SF = *s1 == 1; */
  print_asm_template2(xor);
}//计算用rtl_name，复制用 operand_write

static inline def_EHelper(or) {
  rtl_or(s,s0,ddest,dsrc1);
  operand_write(s,id_dest,s0);
  cpu.eflags.CF=0;
  cpu.eflags.OF=0;
  rtl_update_ZFSF(s,s0,id_dest->width);
  /* rtl_msb(s,s1,s0,id_dest->width);
  cpu.eflags.ZF = *s0 == 0;
  cpu.eflags.SF = *s1 == 1; */

  print_asm_template2(or);
}

static inline def_EHelper(sar) {
  
  // unnecessary to update CF and OF in NEMU
  rtl_mv(s,s0,dsrc1);
  rtl_andi(s,s0,s0,c_shift_mask);
  rtl_mv(s,s1,ddest);
  while(*s0 != 0)
  {
    rtl_andi(s,s2,s1,1);
    cpu.eflags.CF = *s2==1;
    if(s->isa.is_operand_size_16)
    {
      *s1 = (int16_t)*s1 >> 1;
    }
    else {
      rtl_sari(s,s1,s1,1);
    }
    rtl_subi(s,s0,s0,1);
  }
  operand_write(s,id_dest,s1);
  if(*dsrc1 == 1) cpu.eflags.OF = 0;
  cpu.eflags.ZF = *s1 == 0;
  rtl_msb(s,s2,s1,id_dest->width);
  cpu.eflags.SF = *s2 == 1;
  print_asm_template2(sar);
} //算术右移，故使用有符号除法  //当操作数位宽为16时，算术右移出错

static inline def_EHelper(shl) {
  
  // unnecessary to update CF and OF in NEMU
  rtl_mv(s,s0,dsrc1);
  rtl_andi(s,s0,s0,c_shift_mask);
  rtl_mv(s,s1,ddest);
  while(*s0 != 0)
  {
    rtl_msb(s,s2,s1,id_dest->width);
    cpu.eflags.CF = *s2==1;
    rtl_shli(s,s1,s1,1);
    rtl_subi(s,s0,s0,1);
  }
  operand_write(s,id_dest,s1);
  if(*dsrc1 == 1)
  {
    rtl_msb(s,s2,s1,id_dest->width);
    cpu.eflags.OF = *s2 != cpu.eflags.CF;
  }
  cpu.eflags.ZF = *s1 == 0;
  rtl_msb(s,s2,s1,id_dest->width);
  cpu.eflags.SF = *s2 == 1;

  print_asm_template2(shl);
}//shl,sal,左移相同

static inline def_EHelper(shr) {
  
  // unnecessary to update CF and OF in NEMU
  rtl_mv(s,s0,dsrc1);
  rtl_andi(s,s0,s0,c_shift_mask);
  rtl_mv(s,s1,ddest);
  while(*s0 != 0)
  {
    rtl_andi(s,s2,s1,1);
    cpu.eflags.CF = *s2==1;
    rtl_shri(s,s1,s1,1);
    rtl_subi(s,s0,s0,1);
  }
  operand_write(s,id_dest,s1);
  if(*dsrc1 == 1) {
    rtl_msb(s,s2,ddest,id_dest->width);
    cpu.eflags.OF = *s2;//in nemu ,OF is necessary
  }
  cpu.eflags.ZF = *s1 == 0;
  rtl_msb(s,s2,s1,id_dest->width);
  cpu.eflags.SF = *s2 == 1;
  print_asm_template2(sar);
  print_asm_template2(shr);
}//逻辑右移


static inline def_EHelper(setcc) {
  uint32_t cc = s->opcode & 0xf;
  rtl_setcc(s, ddest, cc);
  operand_write(s, id_dest, ddest);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

static inline def_EHelper(not) {
  rtl_not(s,s0,ddest);
  operand_write(s,id_dest,s0);

  print_asm_template1(not);
}


static inline def_EHelper(shld) {
  *s0 = *dsrc1%32;//ShiftAmt
  *s1 = *dsrc2;//register
  if(*s0 == 0);
  else {
    if(s->isa.is_operand_size_16)
    {
      rtl_shl(s,ddest,ddest,s0);
      rtl_andi(s,ddest,ddest,0xffff);
      rtl_andi(s,s1,s1,0xffff);
      rtl_shri(s,s1,s1,16-*s0);
      rtl_or(s,s1,s1,ddest);
      operand_write(s,id_dest,s1);
    }
    else{
      rtl_shl(s,ddest,ddest,s0);
      rtl_shri(s,s1,s1,32-*s0);
      rtl_or(s,s1,s1,ddest);
      operand_write(s,id_dest,s1);
    }
    
  }
}


static inline def_EHelper(shrd)
{
  *s0 = *dsrc1%32;//ShiftAmt
  *s1 = *dsrc2;//register
  if(*s0 == 0);
  else{
    if(s->isa.is_operand_size_16)
    {
      rtl_shr(s,ddest,ddest,s0);
      rtl_andi(s,ddest,ddest,0xffff);
      rtl_andi(s,s1,s1,0xffff);
      rtl_shli(s,s1,s1,16-*s0);
      rtl_or(s,s1,s1,ddest);
      operand_write(s,id_dest,s1);
    }
    else{
      rtl_shr(s,ddest,ddest,s0);
      rtl_shli(s,s1,s1,32-*s0);
      rtl_or(s,s1,s1,ddest);
      operand_write(s,id_dest,s1);
    }
  }
}

/*
t0, t1, ... - 只能在RTL伪指令的实现过程中存放中间结果
s0, s1, ... - 只能在译码辅助函数和执行辅助函数的实现过程中存放中间结果
*/