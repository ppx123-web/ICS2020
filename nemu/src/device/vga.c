#include <common.h>

#ifdef HAS_IOE

#define SHOW_SCREEN
//#define MODE_800x600

#ifdef MODE_800x600
# define SCREEN_W 800
# define SCREEN_H 600
#else
# define SCREEN_W 400
# define SCREEN_H 300
#endif
#define SCREEN_SIZE ((SCREEN_H * SCREEN_W) * sizeof(uint32_t))

#include <device/map.h>
#include <SDL2/SDL.h>

#define VMEM 0xa0000000

#define VGACTL_PORT 0x100
#define VGACTL_MMIO 0xa1000100

static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;

static uint32_t (*vmem) [SCREEN_W] = NULL;
static uint32_t *vgactl_port_base = NULL;

static inline void update_screen() {
#ifdef SHOW_SCREEN
  SDL_UpdateTexture(texture, NULL, vmem, SCREEN_W * sizeof(vmem[0][0]));
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
#endif
}

void vga_update_screen() {
  uint32_t sync = vgactl_port_base[1];
  if(sync == 1)
  {
    update_screen();
    vgactl_port_base[1] = 0;
  }
  //TODO: call 'update_screen()' when the sync register is non-zero,
  //then zero out the sync register
}

void init_vga() {
#ifdef SHOW_SCREEN
  SDL_Window *window = NULL;
  char title[128];
  sprintf(title, "%s-NEMU", str(__ISA__));
  SDL_Init(SDL_INIT_VIDEO);
#ifdef MODE_800x600
  SDL_CreateWindowAndRenderer(SCREEN_W, SCREEN_H, 0, &window, &renderer);
#else
  SDL_CreateWindowAndRenderer(SCREEN_W * 2, SCREEN_H * 2, 0, &window, &renderer);
#endif
  SDL_SetWindowTitle(window, title);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_STATIC, SCREEN_W, SCREEN_H);
#endif

  vgactl_port_base = (void *)new_space(8);
  vgactl_port_base[0] = ((SCREEN_W) << 16) | (SCREEN_H);
  add_pio_map("screen", VGACTL_PORT, (void *)vgactl_port_base, 8, NULL);
  add_mmio_map("screen", VGACTL_MMIO, (void *)vgactl_port_base, 8, NULL);

  vmem = (void *)new_space(SCREEN_SIZE);
  add_mmio_map("vmem", VMEM, (void *)vmem, SCREEN_SIZE, NULL);
}
#endif	/* HAS_IOE */
