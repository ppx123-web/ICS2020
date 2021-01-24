#include <am.h>
#include <x86.h>
#include <klib.h>

#define NR_IRQ 256 // IDT size
#define SEG_KCODE 1
#define SEG_KDATA 2

#define NR_SEG 6
static SegDesc gdt[NR_SEG] = {};
static TSS32 tss = {};


static Context *(*user_handler)(Event, Context *) = NULL;

void __am_irq0();
void __am_vecsys();
void __am_vectrap();
void __am_vecnull();

void __am_get_cur_as(Context *c);
void __am_switch(Context *c);


#define RPL(A) ((A)&0x3)

Context *__am_irq_handle(Context *c)
{
  //printf("in cte\n");
  __am_get_cur_as(c);
  //printf("Context cr3:%x\n",c->cr3);

  if (user_handler)
  {
    Event ev = {0};
    switch (c->irq)
    {
    case 0x80:
      ev.event = EVENT_SYSCALL;
      break;
    case 0x81:
      ev.event = EVENT_YIELD;
      break;
    case 32:
      ev.event = EVENT_IRQ_TIMER;
      break;
    default:
      ev.event = EVENT_ERROR;
      break;
    }
    c = user_handler(ev, c);
    assert(c != NULL);
  }

  //printf("cr3 = %x pc = %x\n",c->cr3,c->eip);
  if(c->cr3 != NULL)
    __am_switch(c);
  return c;
}

bool cte_init(Context *(*handler)(Event, Context *))
{
  static GateDesc32 idt[NR_IRQ];

  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i++)
  {
    idt[i] = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vecnull, DPL_KERN);
  }

  // initialize GDT
  gdt[1] = SEG32(STA_X | STA_R,   0,      0xffffffff, DPL_KERN);
  gdt[2] = SEG32(STA_W,           0,      0xffffffff, DPL_KERN);
  gdt[3] = SEG32(STA_X | STA_R,   0,      0xffffffff, DPL_USER);
  gdt[4] = SEG32(STA_W,           0,      0xffffffff, DPL_USER);
  gdt[5] = SEG16(STS_T32A,     &tss, sizeof(tss) - 1, DPL_KERN);
  set_gdt(gdt, sizeof(gdt[0]) * NR_SEG);
  // initialize TSS
  tss.ss0 = KSEL(2);
  set_tr(KSEL(5));

  // ----------------------- interrupts ----------------------------
  idt[32] = GATE32(STS_IG, KSEL(SEG_KCODE), __am_irq0, DPL_KERN);
  // ---------------------- system call ----------------------------
  idt[0x80] = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vecsys, DPL_USER);
  idt[0x81] = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vectrap, DPL_KERN);

  set_idt(idt, sizeof(idt));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg)
{
  Context *c = (Context *)((uint8_t *)(kstack.end) - sizeof(Context) - sizeof(uintptr_t));
  c->eip = (uintptr_t)entry;
  c->cs = KSEL(1);
  c->cr3 = NULL;
  c->eflags = 0x200;
  c->ss3 = (uintptr_t)arg;
  //uintptr_t *a = (uintptr_t *)(c+1);
  //*a = (uintptr_t)arg; //参数是向后增长的，所以是在end后面加
  //printf("arg = %d %d %d %p\n",(uintptr_t)arg,sizeof(uintptr_t),c->esp3,&c->esp3);
  return c;
}

void yield()
{
  asm volatile("int $0x81");
}

bool ienabled()
{
  return false;
}

void iset(bool enable)
{
}
