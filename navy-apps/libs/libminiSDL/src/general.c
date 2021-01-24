#include <NDL.h>
#include <assert.h>
#include <stdio.h>

uint8_t* keystate = NULL;

int SDL_Init(uint32_t flags) {
  keystate = malloc(256*sizeof(uint8_t));
  memset(keystate,0,sizeof(keystate));
  return NDL_Init(flags);
}

void SDL_Quit() {
  free(keystate);
  NDL_Quit();
}

char *SDL_GetError() {
  return "Navy does not support SDL_GetError()";
}

int SDL_SetError(const char* fmt, ...) {
  //printf("please implement me\n");
  //assert(0);
  return -1;
}

int SDL_ShowCursor(int toggle) {
  //printf("please implement me\n");
  //assert(0);
  return 0;
}

void SDL_WM_SetCaption(const char *title, const char *icon) {
  //printf("please implement me\n");
  //assert(0);
}
