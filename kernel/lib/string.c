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

char*
safebytecpy(u_int addr, const char b) 
{
  u_int addr1 = addr & 0xfffffffc;
  u_int addr2 = addr & ~0xfffffffc;
  u_int v = *(u_int*)addr1;
  u_int mask = 0x000000ff;
  mask = mask << (8 * addr2);
  v = v & (~mask);
  u_int byte = (u_int)b;
  byte = byte << (8 * addr2);
  *(u_int*)addr1 = v | byte;
}

int
strcmp(const char *p, const char *q)
{
  int ret = 0;
	while(!(ret=*(unsigned char*)p - *(unsigned char*)q) && *p)
		p++, q++;

	if (ret < 0)
		return -1;
	else if (ret > 0)
		return 1;
	return 0;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}