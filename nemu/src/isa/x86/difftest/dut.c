#include <isa.h>
#include <monitor/difftest.h>
#include "../local-include/reg.h"
#include "difftest.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  bool flag[10];
  flag[0]=difftest_check_reg("eax",pc,ref_r->eax,cpu.eax);
  flag[1]=difftest_check_reg("ebx",pc,ref_r->ebx,cpu.ebx);
  flag[2]=difftest_check_reg("ecx",pc,ref_r->ecx,cpu.ecx);
  flag[3]=difftest_check_reg("edx",pc,ref_r->edx,cpu.edx);
  flag[4]=difftest_check_reg("esp",pc,ref_r->esp,cpu.esp);
  flag[5]=difftest_check_reg("ebp",pc,ref_r->ebp,cpu.ebp);
  flag[6]=difftest_check_reg("esi",pc,ref_r->esi,cpu.esi);
  flag[7]=difftest_check_reg("edi",pc,ref_r->edi,cpu.edi);
  flag[8]=difftest_check_reg("pc",pc,ref_r->pc,cpu.pc);
  //flag[9]=difftest_check_reg("eflags",pc,ref_r->eflags.eflags_value,cpu.eflags.eflags_value);
  if((cpu.eflags.eflags_value&ref_r->eflags.eflags_value) == cpu.eflags.eflags_value)
    return true;
  /* if(ref_r->eflags.CF == cpu.eflags.CF&&ref_r->eflags.ZF == cpu.eflags.ZF
    &&ref_r->eflags.SF == cpu.eflags.SF&&ref_r->eflags.OF == cpu.eflags.OF
    &&ref_r->eflags.IF == cpu.eflags.IF)
    flag[9] = true;
  else {
    flag[9] = false;
    if(ref_r->eflags.CF != cpu.eflags.CF) Log("eflags CF wrong = %d\n",cpu.eflags.CF);
    if(ref_r->eflags.ZF != cpu.eflags.ZF) Log("eflags ZF wrong = %d\n",cpu.eflags.ZF);
    if(ref_r->eflags.OF != cpu.eflags.OF) Log("eflags OF wrong = %d\n",cpu.eflags.OF);
    if(ref_r->eflags.SF != cpu.eflags.SF) Log("eflags F wrong = %d\n",cpu.eflags.SF);
    if(ref_r->eflags.IF != cpu.eflags.IF) Log("eflags F wrong = %d\n",cpu.eflags.IF);
  } */
  for(int i = 0;i < 9;i++)
  {
    if(!flag[i]) return false;
  }
  return true;
}

void isa_difftest_attach() {
}
