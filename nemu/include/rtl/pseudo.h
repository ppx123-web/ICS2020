#include<rtl/rtl.h>

#ifndef __RTL_PSEUDO_H__
#define __RTL_PSEUDO_H__

#ifndef __RTL_RTL_H__
#error "Should be only included by <rtl/rtl.h>"
#endif

/* RTL pseudo instructions */

static inline def_rtl(li, rtlreg_t* dest, const rtlreg_t imm) {
  rtl_addi(s, dest, rz, imm);
}

static inline def_rtl(mv, rtlreg_t* dest, const rtlreg_t *src1) {
  if (dest != src1) rtl_add(s, dest, src1, rz);
}

static inline def_rtl(not, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- ~src1
  //rtl_not(s,dest,src1);
  *dest = ~(*src1);
}

static inline def_rtl(neg, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- -src1
  *dest = -(*src1);
}

static inline def_rtl(sext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
  *t0 = (*src1 & (1 << (width*8-1))) - 1;  //求出符号位其余为0然后-1的数
  /* rtl_not(s,t0,t0); //取反与src1或
  *t1 = s->isa.is_operand_size_16 ? 0xffff: 0xffffffff ;
  rtl_and(s,t0,t0,t1);
  rtl_or(s,dest,t0,src1); */
  switch(width){
    case 1:*dest=*(int8_t*)src1;break;
    case 2:*dest=*(int16_t*)src1;break;
    case 4:*dest=*(int*)src1;break;
    default:assert(0);
  }
  
}//符号扩展使用了t0的临时寄存器

static inline def_rtl(zext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- zeroext(src1[(width * 8 - 1) .. 0])
  switch(width){
		case 1:*dest=*(uint8_t*)src1;break;
		case 2:*dest=*(uint16_t*)src1;break;
		case 4:*dest=*(unsigned *)src1;break;
		default:assert(0);
  }
}//0的扩展

static inline def_rtl(msb, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- src1[width * 8 - 1]
  //*dest = (*src1)&(1<<(width*8-1))?1:0;
  if(width == 4)
  {
    *dest = *src1 >> 31;
  }
  else if(width == 2){
    rtl_andi(s,dest,src1,0xffff);
    *dest = *dest >> 15;
  }
  else if(width == 1)
  {
    rtl_andi(s,dest,src1,0xff);
    *dest = *dest >> 7;
  }
  else assert(0);
}

#endif


/*
t0, t1, ... - 只能在RTL伪指令的实现过程中存放中间结果
s0, s1, ... - 只能在译码辅助函数和执行辅助函数的实现过程中存放中间结果
*/