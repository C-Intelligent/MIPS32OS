#include "../kernel/user/user.h"

//简单版本删除 只支持强制删除
//不支持递归删除

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    printf("[rm]: rm files...\n");
    exit();
  }

  for(i = 1; i < argc; i++){
    if(unlink(argv[i]) < 0){
      printf("[rm]: %s failed to delete\n", argv[i]);
      break;
    }
  }

  exit();
}