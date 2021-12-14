#pragma once

#include "types.h"

int     strlen(const char *s);
void *      memset(void *dst, int c, u_int n);
void *  memmove(void *dst, const void *src, u_int n);
char * safestrcpy(char *s, const char *t, int n);
char * gets(char *buf, int max);
char * cmdgets(char *buf, int max);
char * safebytecpy(u_int addr, const char b);

//查找字符首次出现的位置（地址） 没找到返回0
char* strchr(const char *s, char c);