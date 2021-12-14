#include "../kernel/user/user.h"
#include "../kernel/user/string.h"

int
main(int argc, char *argv[])
{
  int pid = fork();
  if (0 == pid) {
    int i = 0;
    while (i++ < 10)
    printf("This is child!\n");
    exit();
  }
  int i = 0;
  while (i++ < 10)
    printf("This is father!\n");
  exit();
}
