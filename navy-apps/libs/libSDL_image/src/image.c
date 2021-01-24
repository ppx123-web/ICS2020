#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
  printf("%s\n",filename);
  FILE* fp = fopen(filename,"r");
  assert(fp != NULL);
  fseek(fp,0,SEEK_END);
  int size = ftell(fp);
  char* buf = malloc(size+1);
  fseek(fp,0,SEEK_SET);
  for(int i = 0;i < size;i ++)
  {
    int k = fscanf(fp,"%c",buf+i);
    assert(k != -1);
  }
  SDL_Surface *s;
  s = STBIMG_LoadFromMemory(buf,size);
  fclose(fp);
  free(buf);
  //printf("%s %d\n",filename,size);
  return s;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
