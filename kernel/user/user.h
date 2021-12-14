#pragma once
#include "types.h"

//系统调用
int sysTest();
int exec(char *path, int argc, char **argv);
int write(int, void*, int);
int fork(void);
int exit(void);
int wait(void);
int read(int, void*, int);
int uname(char* name, int len);
int pwd(char* cwd, int len);
int chdir(const char* path);
int pipe(int*);
int close(int fd);
int open(char*, int);
int fstat(int fd, struct stat*);
int dup(int);  //将新fd定向到（最小未使用的fd）
int unlink(char*);
int link(char*, char*);
int mkdir(char*);
int sleep(int t);
int disprocs();
char* sbrk(int);  //向上增长堆区



//库函数
void printf(char *fmt, ...);
void panic(char *fmt, ...);
void *malloc(u_int nbytes);
void disheap(); //打印堆区信息