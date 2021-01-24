#include "cc.h"

static inline void add_internal(DecodeExecState *s)
{
  rtl_add(s,s0,ddest,dsrc1);
  rtl_update_ZFSF(s,s0,id_dest->width);
  rtl_is_add_carry(s,s1,s0,dsrc1);
  rtl_set_CF(s,s1);
  rtl_is_add_overflow(s,s1,s0,ddest,dsrc1,id_dest->width);
  rtl_set_OF(s,s1);
  //todo
}

static inline def_EHelper(add) {
  add_internal(s);
  rtl_add(s,s0,ddest,dsrc1);
  operand_write(s,id_dest,s0);
  print_asm_template2(add);
}


// dest <- sub result
static inline void cmp_internal(DecodeExecState *s) {
  rtl_sub(s, s0, ddest, dsrc1);
  rtl_update_ZFSF(s, s0, id_dest->width);
  rtl_is_sub_carry(s, s1, ddest, dsrc1);
  rtl_set_CF(s, s1);
  rtl_is_sub_overflow(s, s1, s0, ddest, dsrc1, id_dest->width);
  rtl_set_OF(s, s1);
}//设置OF，CF，ZF，SF

static inline def_EHelper(sub) {
  cmp_internal(s);
  rtl_sub(s,s0,ddest,dsrc1);
  operand_write(s,id_dest,s0);
  print_asm_template2(sub);
}

static inline def_EHelper(cmp) {
  rtl_sext(s,dsrc1,dsrc1,id_src1->width);
  cmp_internal(s);
  print_asm_template2(cmp);
}

static inline def_EHelper(inc) {
  //TODO();
  *s1 = 1;
  rtl_add(s,s0,ddest,s1);
  rtl_update_ZFSF(s,s0,id_dest->width);
  rtl_is_add_overflow(s,s2,s0,ddest,s1,id_dest->width);
  rtl_set_OF(s,s2);
  operand_write(s,id_dest,s0);
  print_asm_template1(inc);
}

static inline def_EHelper(incl) {
		//TODO();
		//*ddest+=1;
		//rtl_sm(s,s->isa.mbase,s->isa.moff,ddest,id_dest->width);
		dsrc1=&id_src1->val;
		*dsrc1=1;
		rtl_add(s,s0,ddest,dsrc1);
		rtl_update_ZFSF(s,s0,id_dest->width);
		rtl_is_add_overflow(s,s1,s0,ddest,dsrc1,id_dest->width);
		rtl_set_OF(s,s1);
		operand_write(s,id_dest,s0);
		print_asm_template1(inc);
}


static inline def_EHelper(dec) {
  /* rtl_subi(s,s0,ddest,1);
  if(s->isa.is_operand_size_16)
  {
    cpu.eflags.OF = *ddest == 0x8000;
    rtl_msb(s,s1,s0,id_dest->width);
    cpu.eflags.SF = *s1 == 1;
    cpu.eflags.ZF = *s0 == 0;
    rtl_andi(s,s0,s0,0xffff);
    operand_write(s,id_dest,s0);
  }
  else{
    cpu.eflags.OF = *ddest == 0x80000000;
    rtl_msb(s,s1,s0,id_dest->width);
    cpu.eflags.SF = *s1 == 1;
    cpu.eflags.ZF = *s0 == 0;
    operand_write(s,id_dest,s0);
  }
  print_asm_template1(dec); */
    dsrc1=&id_src1->val;
		*dsrc1=1;
		rtl_sub(s,s0,ddest,dsrc1);
		rtl_update_ZFSF(s,s0,id_dest->width);
		rtl_is_sub_overflow(s, s1, s0, ddest, dsrc1, id_dest->width);
		rtl_set_OF(s, s1);
		operand_write(s,id_dest,s0);
  print_asm_template1(dec);
}

static inline def_EHelper(neg) {
  /* rtl_neg(s,s0,ddest);
  operand_write(s,id_dest,s0);
  print_asm_template1(neg); */
  rtl_sub(s,s0,rz,ddest);
  rtl_update_ZFSF(s,s0,id_dest->width);
  rtl_is_sub_overflow(s, s1, s0, rz, ddest, id_dest->width);
  rtl_set_OF(s, s1);
  if(*ddest==0)cpu.eflags.CF=0;
  else cpu.eflags.CF=1;
  operand_write(s,id_dest,s0);
  print_asm_template1(neg);
}

static inline def_EHelper(adc) {
  rtl_get_CF(s, s0);
  rtl_add(s, s0, dsrc1, s0);
  rtl_add(s, s1, ddest, s0);
  rtl_update_ZFSF(s, s1, id_dest->width);
  rtl_is_add_overflow(s, s2, s1, ddest, dsrc1, id_dest->width);
  rtl_set_OF(s, s2);
  if (id_dest->width != 4) {
    rtl_andi(s, s1, s1, 0xffffffffu >> ((4 - id_dest->width) * 8));
  }
  rtl_is_add_carry(s, s2, s1, s0);
  rtl_is_add_carry(s, s0, s0, dsrc1);
  rtl_or(s, s0, s0, s2);
  rtl_set_CF(s, s0);
  operand_write(s, id_dest, s1);
  print_asm_template2(adc);
}

static inline def_EHelper(sbb) {
  rtl_get_CF(s, s0);
  rtl_add(s, s0, dsrc1, s0);
  rtl_sub(s, s1, ddest, s0);
  rtl_update_ZFSF(s, s1, id_dest->width);
  rtl_is_sub_overflow(s, s2, s1, ddest, dsrc1, id_dest->width);
  rtl_set_OF(s, s2);
  rtl_is_add_carry(s, s2, s0, dsrc1);
  rtl_is_sub_carry(s, s0, ddest, s0);
  rtl_or(s, s0, s0, s2);
  rtl_set_CF(s, s0);
  operand_write(s, id_dest, s1);
  print_asm_template2(sbb);
}

static inline def_EHelper(mul) {
  rtl_lr(s, s0, R_EAX, id_dest->width);
  rtl_mul_lo(s, s1, ddest, s0);

  switch (id_dest->width) {
    case 1:
      rtl_sr(s, R_AX, s1, 2);
      break;
    case 2:
      rtl_sr(s, R_AX, s1, 2);
      rtl_shri(s, s1, s1, 16);
      rtl_sr(s, R_DX, s1, 2);
      break;
    case 4:
      rtl_mul_hi(s, s0, ddest, s0);
      rtl_sr(s, R_EDX, s0, 4);
      rtl_sr(s, R_EAX, s1, 4);
      break;
    default: assert(0);
  }

  print_asm_template1(mul);
}//没有考虑CF，OF

// imul with one operand
static inline def_EHelper(imul1) {
  switch (id_dest->width) {
    case 1:
      rtl_lr(s, s0, R_EAX, 1);
      rtl_imul_lo(s, s1, ddest, s0);
      rtl_sr(s, R_AX, s1, 2);
      break;
    case 2:
      rtl_lr(s, s0, R_EAX, 2);
      rtl_imul_lo(s, s1, ddest, s0);
      rtl_sr(s, R_AX, s1, 2);
      rtl_shri(s, s1, s1, 16);
      rtl_sr(s, R_DX, s1, 2);
      break;
    case 4:
      ; rtlreg_t *pdest = ddest;
      if (ddest == &cpu.edx) {
        rtl_mv(s, s0, ddest);
        pdest = s0;
      }
      rtl_imul_hi(s, &cpu.edx, pdest, &cpu.eax);
      rtl_imul_lo(s, &cpu.eax, pdest, &cpu.eax);
      break;
    default: assert(0);
  }

  print_asm_template1(imul);
}

// imul with two operands
static inline def_EHelper(imul2) {
  rtl_sext(s, dsrc1, dsrc1, id_src1->width);
  rtl_sext(s, ddest, ddest, id_dest->width);

  rtl_imul_lo(s, ddest, ddest, dsrc1);
  operand_write(s, id_dest, ddest);

  print_asm_template2(imul);
}//没有设置CF，OF

// imul with three operands
static inline def_EHelper(imul3) {
  if(s->opcode==0x6b){
		int width = s->isa.is_operand_size_16 ? 2 : 4;
		s->src1.width = s->dest.width = s->src2.width = width;
	}

  rtl_sext(s, dsrc1, dsrc1, id_src1->width);
  rtl_sext(s, dsrc2, dsrc2, id_src1->width);

  rtl_imul_lo(s, ddest, dsrc2, dsrc1);
  operand_write(s, id_dest, ddest);

  print_asm_template3(imul);
}

static inline def_EHelper(div) {
  switch (id_dest->width) {
    case 1:
      rtl_lr(s, s0, R_AX, 2);
      rtl_div_q(s, s1, s0, ddest);
      rtl_sr(s, R_AL, s1, 1);
      rtl_div_r(s, s1, s0, ddest);
      rtl_sr(s, R_AH, s1, 1);
      break;
    case 2:
      rtl_lr(s, s0, R_AX, 2);
      rtl_lr(s, s1, R_DX, 2);
      rtl_shli(s, s1, s1, 16);
      rtl_or(s, s0, s0, s1);
      rtl_div_q(s, s1, s0, ddest);
      rtl_sr(s, R_AX, s1, 2);
      rtl_div_r(s, s1, s0, ddest);
      rtl_sr(s, R_DX, s1, 2);
      break;
    case 4:
      ; rtlreg_t *pdest = ddest;
      if (ddest == &cpu.eax) pdest = s0;
      rtl_mv(s, s0, &cpu.eax);
      rtl_div64_q(s, &cpu.eax, &cpu.edx, s0, pdest);
      rtl_div64_r(s, &cpu.edx, &cpu.edx, s0, pdest);
      break;
    default: assert(0);
  }

  print_asm_template1(div);
}

static inline def_EHelper(idiv) {
  switch (id_dest->width) {
    case 1:
      rtl_lr(s, s0, R_AX, 2);
      rtl_idiv_q(s, s1, s0, ddest);
      rtl_sr(s, R_AL, s1, 1);
      rtl_idiv_r(s, s1, s0, ddest);
      rtl_sr(s, R_AH, s1, 1);
      break;
    case 2:
      rtl_lr(s, s0, R_AX, 2);
      rtl_lr(s, s1, R_DX, 2);
      rtl_shli(s, s1, s1, 16);
      rtl_or(s, s0, s0, s1);
      rtl_idiv_q(s, s1, s0, ddest);
      rtl_sr(s, R_AX, s1, 2);
      rtl_idiv_r(s, s1, s0, ddest);
      rtl_sr(s, R_DX, s1, 2);
      break;
    case 4:
      ; rtlreg_t *pdest = ddest;
      if (ddest == &cpu.eax) pdest = s0;
      rtl_mv(s, s0, &cpu.eax);
      rtl_idiv64_q(s, &cpu.eax, &cpu.edx, s0, pdest);
      rtl_idiv64_r(s, &cpu.edx, &cpu.edx, s0, pdest);
      break;
    default: assert(0);
  }

  print_asm_template1(idiv);
}