#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include <rtl/rtl.h>
#include "reg.h"

/* RTL pseudo instructions */

static inline def_rtl(lr, rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(s, dest, &reg_l(r)); return;
    case 1: rtl_host_lm(s, dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(s, dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline def_rtl(sr, int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(s, &reg_l(r), src1); return;
    case 1: rtl_host_sm(s, &reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(s, &reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

static inline def_rtl(push, const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  /* if(s->isa.is_operand_size_16 )
  {
    cpu.esp-=2;
    rtl_sm(s,&cpu.esp,0,src1,2);
  }
  else {
    if(src1 == &cpu.esp)
    {
      rtl_sm(s,&cpu.esp,-4,src1,4);
      cpu.esp-=4;
    }
    else{
      cpu.esp-=4;
      rtl_sm(s,&cpu.esp,0,src1,4);
    }
    
  } */
  /* if(s->isa.is_operand_size_16) 
  {
    printf("push 16 operand\n pc = %x,data = %x\n",cpu.pc,*src1);
    assert(0);
  } */
  if(src1 == &cpu.esp)
    {
      rtl_sm(s,&cpu.esp,-4,src1,4);
      cpu.esp-=4;
    }
    else{
      cpu.esp-=4;
      rtl_sm(s,&cpu.esp,0,src1,4);
    }
  //printf("pc = %x push %x esp %x\n",cpu.pc,*src1,cpu.esp+4);
}

static inline def_rtl(pop, rtlreg_t* dest) {
  if(s->isa.is_operand_size_16)
  {
    rtl_lm(s,dest,&cpu.esp,0,2);
    cpu.esp+=2;
  }
  else {
    rtl_lm(s,dest,&cpu.esp,0,4);
    cpu.esp+=4;
  }
  //printf("pc = %x pop %x esp %x\n",cpu.pc,*dest,cpu.esp-4);
  // dest <- M[esp]
  // esp <- esp + 4
  //TODO();
} 



static inline def_rtl(is_sub_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  rtl_msb(s,t0,src1,width);
  rtl_msb(s,t1,src2,width);
  rtl_msb(s,dest,res,width);
  if(*t0 != *t1 && *t0 != *dest)
    *dest = 1;
  else *dest = 0;
}

static inline def_rtl(is_sub_carry, rtlreg_t* dest,
    const rtlreg_t* src1, const rtlreg_t* src2) {
  // dest <- is_carry(src1 - src2)
  if(id_dest->width == 1)
  {
    if( (uint8_t)*src1 <  (uint8_t) *src2)
      *dest = 1;
    else *dest = 0;
  }
  else if(s->isa.is_operand_size_16)
  {
    if( (uint16_t)*src1 <  (uint16_t) *src2)
      *dest = 1;
    else *dest = 0;
  }
  else {
    if( (uint32_t) *src1  < (uint32_t) *src2)
      *dest = 1;
    else *dest = 0;
  }

}//There maybe a problem in CF

static inline def_rtl(is_add_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  rtl_msb(s,t0,src1,width);
  rtl_msb(s,t1,src2,width);
  rtl_msb(s,dest,res,width);
  if(*t0 == *t1 && *dest != *t1)
    *dest = 1;
  else *dest = 0;
}

static inline def_rtl(is_add_carry, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  //TODO();
  if(id_dest->width == 1)
  {
    if((uint8_t)*res < (uint8_t)*src1)
      *dest = 1;
    else *dest = 0;
  }else if(s->isa.is_operand_size_16)
  {
    if( (uint16_t)*res < (uint16_t)*src1 )
      *dest = 1;
    else *dest = 0;
  }
  else{
    if( (uint32_t)*res < (uint32_t)*src1 )
      *dest = 1;
    else *dest = 0;
  }
}

#define def_rtl_setget_eflags(f) \
  static inline def_rtl(concat(set_, f), const rtlreg_t* src) { \
  cpu.eflags.f = *src; \
  } \
  static inline def_rtl(concat(get_, f), rtlreg_t* dest) { \
  *dest = cpu.eflags.f; \
  }

def_rtl_setget_eflags(CF)
def_rtl_setget_eflags(OF)
def_rtl_setget_eflags(ZF)
def_rtl_setget_eflags(SF)

static inline def_rtl(update_ZF, const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  switch (width)
  {
  case 1:*t1 = 0xff; break;
  case 2:*t1 = 0xffff; break;
  case 4:*t1 = 0xffffffff; break;
  }
  rtl_and(s,t0,result,t1);
  cpu.eflags.ZF = *t0 == 0;
}

static inline def_rtl(update_SF, const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  rtl_msb(s,t0,result,width);
  cpu.eflags.SF = *t0 == 1;
}

static inline def_rtl(update_ZFSF, const rtlreg_t* result, int width) {
  rtl_update_ZF(s, result, width);
  rtl_update_SF(s, result, width);
}

#endif
