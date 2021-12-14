#include "../kernel/user/user.h"

//简单版本ln

int
main(int argc, char *argv[])
{
  if(argc != 3){
    printf("[ln]: ln old new\n");
    exit();
  }
  if(link(argv[1], argv[2]) < 0)
    printf("[ln] link %s %s: failed\n", argv[1], argv[2]);
  exit();
}
