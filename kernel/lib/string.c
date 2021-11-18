#include "../inc/types.h"
#include "../inc/string.h"

int
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}


char*
safestrcpy(char *s, const char *t, int n)
{
  char *os;
  
  os = s;
  if(n <= 0)
    return os;
  while(--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  return os;
}

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