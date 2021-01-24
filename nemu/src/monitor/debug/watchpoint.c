#include "watchpoint.h"
#include "expr.h"
#include<malloc.h>

#define NR_WP 35

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL, *used =NULL;

WP* new_wp(){
  if(used==head)
  {
    used=free_;
    used->prev=head;
    return used;
  }
  else{
    used->next=free_;
    free_->prev=used;
    used=used->next;
    free_=free_->next;
    return used;
  }
}

void free_wp(WP *wp)
{
  if(wp==used)
  {
    free_->prev=used;
    used->next=free_;
    used=used->prev;
  }
  else{
    if(wp->prev==head)
    {
      wp->next->prev=head;
      wp->next=free_;
      free_->prev=wp;
      free_=free_->prev;
      free(wp->exp);
    }
    else{
      wp->prev->next=wp->next;
      wp->next->prev=wp->prev;
      wp->next=free_;
      free_->prev=wp;
      free_=free_->prev;
      free(wp->exp);
    }
  }
  
  
}//此处未考虑wp_pool使用完的情况，即认为free_ != NULL

void display_wp_pool()
{
  WP* wp=used;
  if(used==head)
  {
    printf("No watchpoint\n");
  }
  else{
    while(wp != head)
    {
      printf("%d %s\n",wp->NO,wp->exp);
      wp=wp->prev;
    }
  }
}

WP* all_wp()
{
  return used;
}

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;
  for(i = 1;i < NR_WP;i ++)
  {
    wp_pool[i].prev=&wp_pool[i-1];
  }
  wp_pool[0].prev=head;
  head = NULL;
  free_ = wp_pool;
  used=NULL;
}

/* TODO: Implement the functionality of watchpoint */

