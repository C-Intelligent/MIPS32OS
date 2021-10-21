#include "../inc/types.h"
#include "../inc/defs.h"

void*
memset(void *dst, int c, u_int n)
{
//   if ((int)dst%4 == 0 && n%4 == 0){
//     c &= 0xFF;
//     stosl(dst, (c<<24)|(c<<16)|(c<<8)|c, n/4);
//   } else
//     stosb(dst, c, n);
    char *p = (char*)dst;
    while (n-- > 0) *(p++) = (char)c;
    return dst;
}

void*
memmove(void *dst, const void *src, u_int n)
{
  const char *s;
  char *d;

  s = src;
  d = dst;
  if(s < d && s + n > d){
    s += n;
    d += n;
    while(n-- > 0)
      *--d = *--s;
  } else
    while(n-- > 0)
      *d++ = *s++;

  return dst;
}