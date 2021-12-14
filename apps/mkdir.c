#include "../kernel/user/user.h"

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    printf("[mkdir]: mkdir files...\n");
    exit();
  }

  for(i = 1; i < argc; i++){
    if(mkdir(argv[i]) < 0){
      printf("[mkdir]: %s failed to create\n", argv[i]);
      break;
    }
  }

  exit();
}