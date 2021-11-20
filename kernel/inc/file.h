#ifndef _file_h_
#define _file_h_

#include "../inc/types.h"
#include "../drivers/ff.h"

struct file{
  enum { FD_NONE, FD_PIPE, FD_FAT, FD_SERIAL } type;
  int ref; // reference count
  char readable;
  char writable;
  //pipe暂时未实现
//   struct pipe *pipe;
  FIL* fp;
//   struct inode *ip;
  u_int off;
};


#endif