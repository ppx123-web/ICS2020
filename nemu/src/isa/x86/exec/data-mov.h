static inline def_EHelper(mov) {
  operand_write(s, id_dest, dsrc1);
  print_asm_template2(mov);
}

static inline def_EHelper(push) {
  rtl_push(s,ddest);
  print_asm_template1(push);
  //if(*ddest == 0x102||*ddest == 0x2)
    //printf("pc = %x push %x esp %x\n",cpu.pc,*ddest,cpu.esp); 
}

static inline def_EHelper(pop) {
  //TODO();
  rtl_pop(s,s0);
  operand_write(s,id_dest,s0);
  print_asm_template1(pop);
  //if(*ddest == 0x102||*ddest == 0x2)
    //printf("pc = %x pop %x esp %x\n",cpu.pc,*ddest,cpu.esp);
}

static inline def_EHelper(pusha) {
  //TODO();
  if(s->isa.is_operand_size_16)
  {
    *s0 = cpu.sp;
    *s1 = cpu.ax;
    rtl_push(s,s1);
    *s1 = cpu.cx;
    rtl_push(s,s1);
    *s1 = cpu.dx;
    rtl_push(s,s1);
    *s1 = cpu.bx;
    rtl_push(s,s1);
    rtl_push(s,s0);
    *s1 = cpu.bp;
    rtl_push(s,s1);
    *s1 = cpu.si;
    rtl_push(s,s1);
    *s1 = cpu.di;
    rtl_push(s,s1);
  }
  else{
    rtl_mv(s,s0,&cpu.esp);
    rtl_push(s,&cpu.eax);
    rtl_push(s,&cpu.ecx);
    rtl_push(s,&cpu.edx);
    rtl_push(s,&cpu.ebx);
    rtl_push(s,s0);
    rtl_push(s,&cpu.ebp);
    rtl_push(s,&cpu.esi);
    rtl_push(s,&cpu.edi);
  }
  //printf("%x %x %x %x\n",cpu.esp,cpu.ebp,cpu.eax,cpu.esi);
  print_asm("pusha");
}

static inline def_EHelper(popa) {
  if(s->isa.is_operand_size_16)
  {
    TODO();
  }
  else{
    rtl_pop(s,&cpu.edi);
    rtl_pop(s,&cpu.esi);
    rtl_pop(s,&cpu.ebp);
    rtl_pop(s,s0);
    rtl_pop(s,&cpu.ebx);
    rtl_pop(s,&cpu.edx);
    rtl_pop(s,&cpu.ecx);
    rtl_pop(s,&cpu.eax);
  }
  //printf("%x %x %x %x\n",cpu.esp,cpu.ebp,cpu.eax,cpu.esi);
  print_asm("popa");
}

static inline def_EHelper(leave) {
  //TODO();
  if(s->isa.is_operand_size_16)
  {
    reg_w(4) = reg_w(5);
    rtl_pop(s,t0);
    reg_w(5) = *t0;
  }
  else {
    reg_l(4) = reg_l(5);
    rtl_pop(s,t0);
    reg_l(5) = *t0;
  }

  print_asm("leave");
}//There may be a problem caseed by push and pop

static inline def_EHelper(cltd) {
  /* if (s->isa.is_operand_size_16) {
    rtl_msb(s,s0,&cpu.eax,2);
    if(*s0 == 1) cpu.dx = 0xffff;
    else cpu.dx = 0;
  }
  else {
    rtl_msb(s,s0,&cpu.eax,4);
    if(*s0 == 1)
      cpu.edx = 0xffffffff;
    else cpu.edx = 0;
  } */
  if (s->isa.is_operand_size_16) {
	  reg_w(R_DX)=(reg_w(R_AX)&(1<<15))==0?0:0xffff;
  }
  else {
    cpu.edx=(cpu.eax&(1<<31))==0?0:0xffffffff;
  }

  print_asm(s->isa.is_operand_size_16 ? "cwtl" : "cltd");
}

static inline def_EHelper(cwtl) {
  /* if (s->isa.is_operand_size_16) {
    *s0 = cpu.al;
    rtl_sext(s,s0,s0,1);
    cpu.ax=*s0;
  }
  else {
    *s0 = cpu.ax;
    rtl_sext(s,s0,s0,2);
    cpu.eax = *s0;
  } */
  if (s->isa.is_operand_size_16) {
    reg_w(R_AX)=*(int8_t*)(&reg_b(R_AL));
  }
  else {
    reg_l(R_EAX)=*(int16_t*)(&reg_w(R_AX));
  }

  print_asm(s->isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

static inline def_EHelper(movsx) {
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  if(s->opcode==0xbf)id_src1->width=2;
  else id_src1->width=1;
  rtl_sext(s, ddest, dsrc1, id_src1->width);
  operand_write(s, id_dest, ddest);
  print_asm_template2(movsx);
}

static inline def_EHelper(movzx) {
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  rtl_zext(s, ddest, dsrc1, id_src1->width);
  operand_write(s, id_dest, ddest);
  print_asm_template2(movzx);
}

static inline def_EHelper(lea) {
  rtl_addi(s, ddest, s->isa.mbase, s->isa.moff);
  operand_write(s, id_dest, ddest);
  print_asm_template2(lea);
}


static inline def_EHelper(movs) 
{
  switch(id_dest->width)
  {
    case 1: 
      rtl_lms(s,s0,&cpu.esi,0,1);
      rtl_sm(s,&cpu.edi,0,s0,1);
      *s1 = 1;
      break;
    case 2:
      rtl_lms(s,s0,&cpu.esi,0,2);
      rtl_sm(s,&cpu.edi,0,s0,2);
      *s1 = 2;
      break;
    case 4:
      rtl_lms(s,s0,&cpu.esi,0,4);
      rtl_sm(s,&cpu.edi,0,s0,4);
      *s1 = 4;
      break;  
  }
  rtl_add(s,&cpu.esi,&cpu.esi,s1);
  rtl_add(s,&cpu.edi,&cpu.edi,s1);
}

/* static inline def_EHelper(movsb) {
  rtl_lms(s,s0,&cpu.esi,0,1);
  rtl_lms(s,s1,&cpu.edi,0,1);
  rtl_andi(s,s1,s1,0xffffff00);
  rtl_add(s,s1,s1,s0);
  rtl_sm(s,&cpu.edi,0,s1,1);
  rtl_addi(s,&cpu.esi,&cpu.esi,1);
  rtl_addi(s,&cpu.edi,&cpu.edi,1);

}//($esi) -> ($edi)


static inline def_EHelper(movsl)
{
  if(s->isa.is_operand_size_16)
  {
    rtl_lms(s,s0,&cpu.esi,0,2);
    rtl_sm(s,&cpu.edi,0,s0,2);
    *s1 = 2;
  }
  else{
    rtl_lms(s,s0,&cpu.esi,0,4);
    rtl_sm(s,&cpu.edi,0,s0,4);
    *s1 = 4;
    if(cpu.eflags.DF == 0) *s1 = 4;
    else *s1 = -4;
  }
  rtl_add(s,&cpu.esi,&cpu.esi,s1);
  rtl_add(s,&cpu.edi,&cpu.edi,s1);
} 
 */
static inline def_EHelper(stos)
{
  if(s->isa.is_operand_size_16)
  {
    *s0 = cpu.eax&0xffff;
    rtl_andi(s,s1,&cpu.edi,0xffff);
    rtl_sm(s,s1,0,s0,2);
    if(cpu.eflags.DF == 0) cpu.di+=2;
    else cpu.di-=2;
  }
  else{//byte type instruction
    *s0 = cpu.al;
    rtl_andi(s,s1,&cpu.edi,0xffff);
    rtl_sm(s,s1,0,s0,2);
    if(cpu.eflags.DF == 0) cpu.edi += 1;
    else cpu.edi -= 1;
  }
}

static inline def_EHelper(bsr) {
  if(*dsrc1 == 0)
  {
    cpu.eflags.ZF = 0;
  }
  else{
    *s0 = (s->isa.is_operand_size_16?16:32) - 1;
    cpu.eflags.ZF = 1;
    while((*dsrc1 & (1<<*s0)) == 0 )
    {
      rtl_subi(s,s0,s0,1);
      *ddest = *s0;
    }
  }
}

