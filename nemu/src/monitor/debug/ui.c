#include <isa.h>
#include "expr.h"
#include "watchpoint.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <monitor/monitor.h>
#include "memory/paddr.h"

void cpu_exec(uint64_t);
int is_batch_mode();
void display_wp_pool();
void free_wp(WP *wp);
WP* new_wp();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  nemu_state.state=NEMU_QUIT;
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args)
{
  if(args==NULL)
  {
    cpu_exec(1);
    return 0;
  }
    
  uint32_t instructions_num1=0;
  for(int i = 0;i < strlen(args); i ++)
    instructions_num1=instructions_num1*10+*(args+i)-'0';  //计算执行的步数
  cpu_exec(instructions_num1);  
  return 0;     
}//单步执行指令

static int cmd_p(char *args)
{
  bool flag=true;
  int val = expr(args,&flag);
  printf("%d\n",val);
  if(!flag)
  {
    printf("Wrong format!\n");
    return -1;
  }
  return 0;
}

static int cmd_x(char * args)
{
  
  char* args_copy1=strtok(args," ");
  char* args_temp=args_copy1+strlen(args_copy1)+1;
  int num2=atoi(args_copy1);
  bool flag=true;
  uint32_t place=expr(args_temp,&flag);
  if(flag)
  {
    for( int i = 0; i < 4*num2;i=i+4)
    {
      printf("%x\n",paddr_read(place+i,4));
    }   
  }
  else{
    printf("Wrong format!\n");
  }
  return 0;
}     

static int cmd_info(char * args)
{
  if(args!=NULL && *args=='r')
  {
    isa_reg_display();//打印寄存器信息（十进制）
  }
  else if(args!=NULL && *args=='w')
  {
    display_wp_pool();
  }
  return 0;
}

static int cmd_w(char * args)
{
  WP* wp=new_wp();
  wp->exp=(char*)malloc(sizeof(char)*(strlen(args)+10));
  strcpy(wp->exp,args);
  return 0;
}//设置监视点

static int cmd_d(char* args)
{
  return 0;
}//删除u监视点

int is_difftest = 0;

static int cmd_detach()
{
  is_difftest = 1;
  assert(is_difftest == 1);
  return 0;
}

//void set_state();

static int cmd_attach()
{
  is_difftest = 0;
  assert(is_difftest == 0);
  //set_state();
  return 0;
}


static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "让程序单步执行N条指令后暂停执行，当N没有给出时，缺省为1", cmd_si},
  { "info", "打印寄存器状态，打印监视点信息",cmd_info},
  { "p", "求出表达式EXPR的值",cmd_p},
  { "x", "求出表达式EXPR的值，将结果作为起始内存地址，以十六进制形式输出连续的N个4字节",cmd_x},
  { "w", "当表达式EXPR的值发生变化时，暂停程序执行",cmd_w},
  { "d", "删除序号为N的监视点" ,cmd_d},
  { "detach", "退出difftest",cmd_detach},
  { "attach", "进入difftest",cmd_attach}

  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}    //用于help + instruction 查询命令

void ui_mainloop() {
  if (is_batch_mode()) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }   //指向命令后的参数，判断是否存在

#ifdef HAS_IOE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }       //判断是哪个命令，在判断命令的参数是否有效，即不小于0,所以在cmd函数中采用了return 0作为结尾。
    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
