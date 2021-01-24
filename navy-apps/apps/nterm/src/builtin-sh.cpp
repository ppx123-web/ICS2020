#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

#define cmd_num 2

char *cmd_table[] = {
  {"ls"},
  {"./"},
};

static void sh_handle_cmd(const char *cmd) {
  if(!strncmp(cmd,cmd_table[0],2)) {
    sh_printf("/bin/bird\n/bin/nterm\n/bin/pal\n/bin/nslider\n/bin/menu\n");
  }
  else if(!strncmp(cmd,cmd_table[1],2)){
    char*str = (char*)malloc(sizeof(cmd)+10);
    memset(str,0,sizeof(str));
    //char* argv[] = {"/bin","/bin",NULL};
    assert( setenv("/bin","0",0) == 0);
    /* char* p= getenv("/bin");
    assert(p != NULL); */
    //printf("%s\n",p);
    strcpy(str,cmd+2);
    char* s = str;
    while(*s == ' ') s++;
    str[strlen(str)-1] = '\0';
    //printf("%s\n",s);
    execvp(s,NULL);
  }
  else{
    sh_printf("wrong command\n");
  }
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();
  //argv[0] = (char*)malloc(100);
  //assert( setenv("/bin","/bin",0) == 0);
  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      //printf("1\n");
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
