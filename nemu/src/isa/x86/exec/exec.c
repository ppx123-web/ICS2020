#include <cpu/exec.h>
#include "../local-include/decode.h"
#include "all-instr.h"

static inline void set_width(DecodeExecState *s, int width) {
  if (width == -1) return;
  if (width == 0) {
    width = s->isa.is_operand_size_16 ? 2 : 4;
  }
  s->src1.width = s->dest.width = s->src2.width = width;
}//设置指令的宽度

/*
t0, t1, ... - 只能在RTL伪指令的实现过程中存放中间结果
s0, s1, ... - 只能在译码辅助函数和执行辅助函数的实现过程中存放中间结果
*/

/* 0x80, 0x81, 0x83 */
static inline def_EHelper(gp1) {
  switch (s->isa.ext_opcode) {
    EXW(0x0,add,-1)//add
    EXW(1,or,-1)
    EXW(0x2,adc,-1)//case 2: break;//adc
    EXW(0x3,sbb,-1)//case 3: break;//sbb
    EXW(0x4,and,-1)
    EXW(0x5,sub,-1)
    EXW(0x6,xor,-1)
    EXW(0x7,cmp,-1)
/*     case 1:
      exec_or(s);
      break; //case 1: break;//or
    case 4:
      exec_and(s);
      break;//and  同时影响着flags，
    case 5:
      exec_sub(s);
      break;//sub   //由于译码阶段已经对数是有符号还是无符号做了翻译
    case 6:
      exec_xor(s);
      break;//xor 
    case 7:
      exec_cmp(s);
      //printf("%d %d %d %d\n",*ddest,*dsrc1,cpu.eflags.CF,cpu.eflags.ZF);
      break;//cmp */
  }
  
  
}// EMPTY为非法的op         there is a problem   

/* 0xc0, 0xc1, 0xd0, 0xd1, 0xd2, 0xd3 */
static inline def_EHelper(gp2) {
  switch (s->isa.ext_opcode) {
    case 0:
      //rtl_andi(s,s0,dsrc1,0xff);
      rtl_mv(s,s0,dsrc1);
      while(*s0 != 0)
      {
        if(s->isa.is_operand_size_16)
        {
          rtl_msb(s,s1,ddest,2);
          *ddest = *ddest * 2 +*s1;
          rtl_andi(s,ddest,ddest,0xffff);
        }
        else{
          rtl_msb(s,s1,ddest,4);
          *ddest = *ddest * 2 +*s1;
        }
        rtl_subi(s,s0,s0,1);
      }
      break;//rol
    case 1:
      rtl_mv(s,s0,dsrc1);
      while(*s0 != 0)
      {
        if(s->isa.is_operand_size_16)
        {
          rtl_andi(s,s1,ddest,1);
          *ddest = *ddest / 2 +((*s1)<<15);
          rtl_andi(s,ddest,ddest,0xffff);
        }
        else{
          rtl_andi(s,s1,ddest,1);
          *ddest = *ddest / 2 +(*s1 << 31);
        }
        rtl_subi(s,s0,s0,1);
      }
      break;//ror
    EMPTY(2) 
    EMPTY(3)
    EXW(4,shl,-1)
    EXW(5,shr,-1)
    EMPTY(6)
    EXW(7,sar,-1)
/*     case 4: 
      exec_shl(s);
      break; //shl ,sal
    case 5:
      exec_shr(s);
      break;//shr   逻辑右移
    case 7:
      exec_sar(s);
      break;//sar  算术右移 */
  }
}

/* 0xf6, 0xf7 */
static inline def_EHelper(gp3) {
  switch (s->isa.ext_opcode) {
    case 0:
      decode_op_I(s,id_src1,true);
      exec_test(s);
      break; //test 
    EMPTY(1) 
    EXW(0x2,not,-1)//not
    EXW(0x3,neg,-1)
    EXW(0x4,mul,-1)//mul
    EXW(0x5,imul1,-1)//imul
    EXW(0x6,div,-1)//div
    EXW(0x7,idiv,-1)//idiv
  }
}//出问题

/* 0xfe */
static inline def_EHelper(gp4) {
  switch (s->isa.ext_opcode) {
    EXW(0,inc,-1)
    EXW(1,dec,-1)
    EMPTY(2) EMPTY(3)
    EMPTY(4) EMPTY(5) EMPTY(6) EMPTY(7)
  }
}

/* 0xff */
static inline def_EHelper(gp5) {
  switch (s->isa.ext_opcode) {
    EX(0x0,incl)//inc Ev
    EX(0x1,dec) //case 1:  break; //dec Ev
    EX(0x2,call_rm)//call Ev
    EMPTY(3) //call eP
    EX(0x4,jmp_rm) //JMP Ev
    EMPTY(5) //case 5: break; //JMP Ep
    EXW(0x6,push,-1) //push Ev
    EMPTY(7) //case 7: break;
  }
}

static inline def_EHelper(gp6) {
  switch (s->isa.ext_opcode) {
    EMPTY(0)
    EMPTY(1) 
    EMPTY(2)
    EXW(0x3,ltr,-1);
    EMPTY(4) 
    EMPTY(5) 
    EMPTY(6) 
    EMPTY(7)
  }
}


/* 0x0f 0x01*/
static inline def_EHelper(gp7) {
  switch (s->isa.ext_opcode) {
    EMPTY(0)
    EMPTY(1) 
    EXW(0x2,lgdt,-1)
    EXW(0x3,lidt,-1)
    EMPTY(4) 
    EMPTY(5) 
    EMPTY(6) 
    EMPTY(7)
  }
}

static inline def_EHelper(2byte_esc) {
  uint8_t opcode = instr_fetch(&s->seq_pc, 1);
  s->opcode = opcode;
  switch (opcode) {
  /* TODO: Add more instructions!!! */
    IDEX (0x01, gp7_E, gp7)
    IDEXW(0x92,setcc_E,setcc,1)
    IDEXW(0x93,setcc_E,setcc,1)
    IDEXW(0x94,setcc_E,setcc,1) //sete or setz
    IDEXW(0x95,setcc_E,setcc,1)  //setne or setnz
    IDEXW(0x96,setcc_E,setcc,1)
    IDEXW(0x97,setcc_E,setcc,1)
    IDEXW(0x99,setcc_E,setcc,1)
    IDEXW(0x9c,setcc_E,setcc,1)
    IDEXW(0x9d,setcc_E,setcc,1)
    IDEXW(0x9e,setcc_E,setcc,1)
    IDEXW(0x9f,setcc_E,setcc,1)
    IDEXW(0xb6,mov_E2G,movzx,1)
    IDEXW(0xbe,mov_E2G,movsx,1)
    IDEXW(0xbf,mov_E2G,movsx,2)
    IDEXW(0xb7,mov_E2G,movzx,2)
    IDEX(0xaf,E2G,imul2)
    IDEX(0x85,J,jcc)   //jne   jnz
    IDEX(0x8d,J,jcc)   //jge  jnl
    IDEX(0x84,J,jcc)   //jz
    IDEX(0x88,J,jcc)    //js
    IDEX(0x89,J,jcc)   //jns
    IDEX(0x87,J,jcc)   //ja
    IDEX(0x8e,J,jcc)   //jle
    IDEX(0x86,J,jcc)   //jbe
    IDEX(0x83,J,jcc)   //jae
    IDEX(0x8f,J,jcc)
    IDEX(0x8c,J,jcc)
    IDEX(0x82,J,jcc)
    IDEX(0xbd,E2G,bsr)
    IDEX(0xa4,Ib_G2E,shld)
    IDEX(0xa5,cl_G2E,shld)
    IDEX(0xac,Ib_G2E,shrd)
    IDEX(0xad,cl_G2E,shrd)

    IDEX(0x22,E2G,mov_r2cr)
    IDEX(0x20,G2E,mov_cr2r)
    
    IDEXW(0x0,E,gp6,2);//gp6
    default: exec_inv(s);
  }
}


static inline void fetch_decode_exec(DecodeExecState *s) {
  uint8_t opcode;
again:     //goto语句，循环
  opcode = instr_fetch(&s->seq_pc, 1);//取指令，然后将s->seq_pc移到下一个字节
  s->opcode = opcode;
  switch (opcode) {
    EX   (0x0f, 2byte_esc)        //OF B3  Save bit in carry flag and reset
    IDEXW(0x80, I2E, gp1, 1)
    IDEX (0x81, I2E, gp1)
    IDEX (0x83, SI2E, gp1)
    IDEXW(0x88, mov_G2E, mov, 1)    //MOV r/m8,r8
    IDEX (0x89, mov_G2E, mov)       //MOV r/m16/32,r16/32
    IDEXW(0x8a, mov_E2G, mov, 1)    //MOV r8,r/m8
    IDEX (0x8b, mov_E2G, mov)       //MOV r16/32,r/m16/32
    IDEXW(0xa0, O2a, mov, 1)        //MOV AL,moffs8     Move byte at (seg:offset) to AL       
    IDEX (0xa1, O2a, mov)           //MOV EAX,moffs32   Move dword at (seg:offset) to EAX
    IDEXW(0xa2, a2O, mov, 1)        //MOV moffs8,AL     Move AL to (seg:offset)
    IDEX (0xa3, a2O, mov)           //MOV moffs16/32,AX   Move AL/eax to (seg:offset)
    IDEXW(0xb0, mov_I2r, mov, 1)    //MOV reg8,imm8     
    IDEXW(0xb1, mov_I2r, mov, 1)    
    IDEXW(0xb2, mov_I2r, mov, 1)
    IDEXW(0xb3, mov_I2r, mov, 1)
    IDEXW(0xb4, mov_I2r, mov, 1)
    IDEXW(0xb5, mov_I2r, mov, 1)
    IDEXW(0xb6, mov_I2r, mov, 1)
    IDEXW(0xb7, mov_I2r, mov, 1)
    IDEX (0xb8, mov_I2r, mov)        //MOV reg16,imm16 /MOV reg32,imm32
    IDEX (0xb9, mov_I2r, mov)
    IDEX (0xba, mov_I2r, mov)
    IDEX (0xbb, mov_I2r, mov)
    IDEX (0xbc, mov_I2r, mov)
    IDEX (0xbd, mov_I2r, mov)
    IDEX (0xbe, mov_I2r, mov)
    IDEX (0xbf, mov_I2r, mov)
    IDEXW(0xc0, gp2_Ib2E, gp2, 1)
    IDEX (0xc1, gp2_Ib2E, gp2)
    IDEXW(0xc6, mov_I2E, mov, 1)
    IDEX (0xc7, mov_I2E, mov)
    IDEXW(0xd0, gp2_1_E, gp2, 1)
    IDEX (0xd1, gp2_1_E, gp2)
    IDEXW(0xd2, gp2_cl2E, gp2, 1)
    IDEX (0xd3, gp2_cl2E, gp2)
    EX   (0xd6, nemu_trap)
    IDEXW(0xf6, E, gp3, 1)    
    IDEX (0xf7, E, gp3)             
    IDEXW(0xfe, E, gp4, 1)          
    IDEX (0xff, E, gp5)                 
  case 0x66: s->isa.is_operand_size_16 = true; goto again;
  case 0x90: break; //nop,next instruction
    
  //add instructions
  //注意：seq_pc是一个地址，是存储opcode的地址，单位为1个字节
    IDEX(0xe8,J,call)     //call rel32
    IDEX(0x50,r,push)     //push
    IDEX(0x51,r,push)     //push
    IDEX(0x52,r,push)
    IDEX(0x53,r,push)
    IDEX(0x54,r,push)
    IDEX(0x55,r,push)
    IDEX(0x56,r,push)
    IDEX(0x57,r,push)     //push register, decide which register
    IDEX(0x68,push_SI,push)     //push imm
    IDEXW(0x6a,push_SI,push,1)   
    EX(0x60,pusha)
    IDEX(0x58,r,pop)  //eax  or  ax
    IDEX(0x59,r,pop)  //
    IDEX(0x5a,r,pop)
    IDEX(0x5b,r,pop)
    IDEX(0x5c,r,pop)
    IDEX(0x5d,r,pop)
    IDEX(0x5e,r,pop)
    IDEX(0x5f,r,pop)   //pop register r decide which register
    //IDEX(0x1f,DS,pop)
    EX(0x61,popa)
    EX(0xc3,ret)          //ret      
    EX(0xc9,leave)           //leave                   
    EX(0xcf,iret)
    IDEX(0x8d,lea_M2G,lea)  //leq M2G
    IDEX(0x3b,E2G,cmp)     //注意rightsrc的符号位扩展
    IDEX(0x39,G2E,cmp)    //cmp $ $
    IDEX(0x3d,I2a,cmp)  //cmp eax ,i
    IDEXW(0x3c,I2a,cmp,1)
    IDEXW(0x38,G2E,cmp,1)
    IDEXW(0x3a,E2G,cmp,1)

    IDEX(0x40,r,inc)
    IDEX(0x41,r,inc)
    IDEX(0x42,r,inc)
    IDEX(0x43,r,inc)
    IDEX(0x44,r,inc)
    IDEX(0x45,r,inc)
    IDEX(0x46,r,inc)
    IDEX(0x47,r,inc)         //incremnet
    IDEX(0x48,r,dec)
    IDEX(0x49,r,dec)
    IDEX(0x4a,r,dec)
    IDEX(0x4b,r,dec)
    IDEX(0x4c,r,dec)
    IDEX(0x4d,r,dec)
    IDEX(0x4e,r,dec)
    IDEX(0x4f,r,dec)        //dec  register
    IDEXW(0x74,J,jcc,1)     //jcc jump  je    1
    IDEXW(0x75,J,jcc,1)     //jne  jnz      1  //there a problem 
    IDEXW(0x7d,J,jcc,1)     //jge ,jnl
    IDEXW(0x7e,J,jcc,1)     //jle  jng      1
    IDEXW(0x7c,J,jcc,1)     //jl jnge  
    IDEXW(0x7f,J,jcc,1)     //jg  jnle
    IDEXW(0x76,J,jcc,1)     //jbe,jna
    IDEXW(0x77,J,jcc,1)     //ja  jnbe
    IDEXW(0x78,J,jcc,1)     //js  
    IDEXW(0x79,J,jcc,1)     //jns
    IDEXW(0x72,J,jcc,1)     //jb
    IDEXW(0x73,J,jcc,1)
    IDEXW(0xeb,J,jmp,1)
    IDEX(0xe9,J,jmp)

    IDEX(0x03,E2G,add)       //add E2G  M/r32/16 -> r32/16 
    IDEX(0x13,E2G,adc)       //add with CF
    IDEX(0x09,G2E,or)
    IDEXW(0x0a,E2G,or,1)
    IDEX(0x0b,E2G,or)
    IDEX(0x0d,I2a,or)
    IDEXW(0x08,G2E,or,1)

    IDEXW(0x84,G2E,test,1)   //test  m/r 8
    IDEX(0x85,G2E,test) 
    IDEXW(0xa8,I2a,test,1)
    IDEX(0xa9,I2a,test)
    IDEXW(0x30,G2E,xor,1)
    IDEX(0x31,G2E,xor)    //xor R xor R   
    IDEXW(0x32,E2G,xor,1)
    IDEX(0x35,I2a,xor)
    IDEX(0x33,E2G,xor)

    IDEX(0x20,G2E,and)
    IDEX(0x21,G2E,and)
    IDEXW(0x22,E2G,and,1)    
    IDEX(0x23,E2G,and)
    IDEXW(0x24,I2a,and,1)
    IDEX(0x25,I2a,and)

    IDEX(0x2d,I2a,sub)
    IDEX(0x29,G2E,sub)
    IDEX(0x2b,E2G,sub)     //sub
    IDEX(0x28,G2E,sub)
    IDEX(0x1b,E2G,sbb)
    IDEXW(0x18,G2E,sbb,1)
    IDEX(0x19,G2E,sbb)
    IDEXW(0x0,G2E,add,1)
    IDEX(0x01,G2E,add)       //add 
    IDEXW(0x2,E2G,add,1)
    //IDEXW(0x04,I2a,add,1)
    IDEX(0x5,I2a,add)
    //IDEXW(0x10,G2E,adc,1)
    IDEX(0x11,G2E,adc)
    IDEX(0x69,I_E2G,imul3)
    IDEXW(0x6b,I_E2G,imul3,1)
    EX(0x99,cltd)
    EX(0x98,cwtl)
    case 0xf3:
      s->seq_pc +=3;
      break;
    case 0x3e: break;
    IDEXW(0xee,out_a2dx,out,1)
    IDEX(0xef,out_a2dx,out)
    IDEX(0xed,in_dx2a,in)

    IDEXW(0xcd,I,int,1)
    EXW(0xa4,movs,1)
    EX(0xa5,movs)
    //EX(0xaa,stos)
    //EX(0xab,stos)
  default: exec_inv(s);
  }   //第一个参数用于作为opcode
}


//注意是否需要的指令都写了，不要出现指令进去是空的情况。


//处理指令形式过多的问题  
/*
1. 前两个字节是0x0f和0x38时，表示需要再读入一个字节才能决定具体的指令形式
2. 使用ModR/M字节中的扩展opcode堆其长度进行扩充
*/

void query_intr(DecodeExecState *s);

vaddr_t isa_exec_once() {
  DecodeExecState s;
  s.is_jmp = 0;
  s.isa = (ISADecodeInfo) { 0 };
  s.seq_pc = cpu.pc; //cpu.pc是下个指令存放的位置

  fetch_decode_exec(&s);
  update_pc(&s);

  query_intr(&s);

  return s.seq_pc;
}
