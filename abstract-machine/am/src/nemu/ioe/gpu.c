#include <am.h>
#include <nemu.h>
#include <string.h>
//#include <klib.h>
#define SYNC_ADDR (VGACTL_ADDR + 4)

#define N 32

#ifdef MODE_800x600
# define W 800
# define H 600
#else
# define W 400
# define H 300
#endif

void __am_gpu_init() {
  /* int i;
  int w = W;
  outw(VGACTL_ADDR,w);
  int h = H;
  outw(VGACTL_ADDR,h);
  uint32_t *fb = (uint32_t*)(uintptr_t)FB_ADDR;
  for(i = 0;i < w*h;i++) fb[i] = 0;
  outl(SYNC_ADDR,1); */
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = inw(VGACTL_ADDR+2), .height = inw(VGACTL_ADDR),
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  uint32_t *fb = (uint32_t*)(uintptr_t)FB_ADDR;
  int x = ctl->x, y = ctl->y, w = ctl->w,h = ctl->h;
  uint32_t *pixels = ctl->pixels;
  for(int i = 0;i < h;i++)
    for(int j = 0;j < w;j++)
    {
      if(y+i < H&&x+j <W)
      {
        fb[W*(y+i)+x+j] = pixels[w*i+j];
        //printf("%x\n",fb[W*(y+i)+x+j]);
      }
    }
  /* if(!ctl->sync)
    printf("finish x=%d y=%d w=%d h=%d\n",x,y,w,h);   */
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}

