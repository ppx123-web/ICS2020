#include <am.h>
#include <nemu.h>
#include <time.h>
#include <sys/time.h>

static uint32_t origin_time_high,origin_time_low;

void __am_timer_init() {
  origin_time_high = inl(RTC_ADDR+4);
  origin_time_low = inl(RTC_ADDR);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uptime->us = (inl(RTC_ADDR+4) - origin_time_high)*1000000 + (inl(RTC_ADDR) - origin_time_low + 500);
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
