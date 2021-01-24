#include <NDL.h>
#include <SDL.h>


int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
  //printf("please implement me\n");
  //assert(0);
  return 0;
}

void SDL_CloseAudio() {
  //printf("please implement me\n");
  //assert(0);
}

void SDL_PauseAudio(int pause_on) {
  //printf("please implement me\n");
  //assert(0);
}

void SDL_MixAudio(uint8_t *dst, uint8_t *src, uint32_t len, int volume) {
  //printf("please implement me\n");
  //assert(0);
}

SDL_AudioSpec *SDL_LoadWAV(const char *file, SDL_AudioSpec *spec, uint8_t **audio_buf, uint32_t *audio_len) {
  //printf("please implement me\n");
  //assert(0);
  return NULL;
}

void SDL_FreeWAV(uint8_t *audio_buf) {
  //printf("please implement me\n");
  //assert(0);
}

void SDL_LockAudio() {
  //printf("please implement me\n");
  //assert(0);
}


void SDL_UnlockAudio() {
  //printf("please implement me\n");
  //assert(0);
}
