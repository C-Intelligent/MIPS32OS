#include "../kernel/user/user.h"
#include "../kernel/user/types.h"
#include "../kernel/user/string.h"

/*
char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;
  
  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  
  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}
*/

void
ls(char *path)
{
  int fd;
  struct stat st;

  if(strlen(path) > PATH_MAX_LENGTH){
    printf("ls: path too long\n");
    return;
  }
  
  if((fd = open(path, READ)) < 0){
    printf("ls: cannot open %s\n", path);
    return;
  }
  
  if(fstat(fd, &st) < 0){
    printf("ls: cannot stat %s\n", path);
    close(fd);
    return;
  }
  
  switch(st.type){
  case T_FIL:
    //printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
    printf("%s\n", st.info.fname);
    break;
  
  case T_DIR:
    while(read(fd, &st.info, sizeof(st.info)) == sizeof(st.info)){
      // printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
      printf("%s ", st.info.fname);
    }
    printf("\n");
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit();
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit();
}
