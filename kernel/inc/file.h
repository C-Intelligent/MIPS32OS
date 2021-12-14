#ifndef _file_h_
#define _file_h_

#include "../inc/types.h"
#include "../drivers/ff.h"

//#define T_DIR 0
//#define T_FIL 1

typedef struct 
{
  union {
    FIL fil;
    DIR dir;
  } ptr_u;
  enum { T_NONE, T_DIR, T_FIL } type;
} FNODE;

struct file{
  enum { FD_NONE, FD_PIPE, FD_FAT, FD_SERIAL } type;
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe;
  FNODE *fp;
  u_int off;
};

typedef struct {
  FNODE fnode;
  char path[128];
} filinfo;


#endif