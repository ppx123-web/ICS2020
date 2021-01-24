#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...)
{
  char ans[2000]={0};
  va_list ap;
  int ret = 0;
  va_start(ap, fmt);
  ret = vsprintf(ans, fmt, ap);
  va_end(ap);
  for(int i = 0; ans[i] != '\0';i++)
  {
    putch(ans[i]);
  }
  return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap)
{
  size_t j = 0, k = 0 ,i = 0;
  int val, num[64] = {0};
  uint32_t v;
  const char* str;
  char nums[20];
  while (*fmt != '\0')
  {
    switch (*fmt)
    {
    case '%':
      //精度,中间还没留精度的选项
      fmt++;
      if (*fmt >= 'a' && *fmt <= 'z')
      {
        switch (*fmt)
        {
          case 'd':
            val = va_arg(ap, int);
            k = 0;
            if (val == 0x80000000)
            {
              out[j++] = '-';out[j++] = '2';out[j++] = '1';out[j++] = '4';out[j++] = '7';out[j++] = '4';
              out[j++] = '8';out[j++] = '3';out[j++] = '6';out[j++] = '4';out[j++] = '8';
              fmt++;
              break;
            }
            else if (val < 0)
            {
              val = (-1) * val;
              out[j++] = '-';
            }
            do
            {
              num[k++] = val % 10 + '0';
              val = val / 10;
            } while (val != 0);
            for (int ii = k - 1; ii >= 0; ii--)
            {
              out[j++] = num[ii];
            }
            fmt++;
            break;
          case 's':
            str = va_arg(ap, char *);
            i= 0;
            while(str[i] != '\0')
            {
              out[j++] = str[i++];
            }
            fmt++;
            break;  
          case 'x':  
            v = va_arg(ap,uint32_t);
            k = 0;
            out[j++]='0';
            out[j++]='x';
            if(v == 0){
                out[j++] = '0';
                fmt++;
                break;
            }
            while(v != 0)
            {
              nums[k++] = v%16 < 10? v%16+'0':'a'+v%16-10;
              v = v/16;
            }
            for(int ii=k-1;ii>=0;ii--){
              out[j++]=nums[ii];
            }
            fmt++;
            break; 
          case 'p':
            v = va_arg(ap,uint32_t);
            k = 0;
            out[j++]='0';
            out[j++]='x';
            if(v == 0){
              out[j++] = '0';
              fmt++;
              break;
            }
            while(v != 0)
            {
              nums[k++] = v%16 < 10? v%16+'0':'a'+v%16-10;
              v = v/16;
            }
            for(int ii=k-1;ii>=0;ii--){
              out[j++]=nums[ii];
            }
            fmt++;
            break;
          case 'c':
            out[j++] = va_arg(ap,int);
            fmt++;
            break;  
          default:
            assert(0);
          //other fuctions remaining to be realized.
        }
      } //处理标志符及一个字母中间的数
      else{
        out[j++] = *fmt;
        fmt++;
      }
      break;
    default:
      out[j++] = *fmt;
      fmt++;
    }
  }
  out[j] = '\0';
  return j;
}

int sprintf(char *out, const char *fmt, ...)
{
  va_list ap;
  int ret = 0;
  va_start(ap, fmt);
  ret = vsprintf(out, fmt, ap);
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...)
{
  return 0;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap)
{
  return 0;
}

#endif
