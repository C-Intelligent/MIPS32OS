#include "../kernel/user/user.h"
#include "../kernel/user/string.h"

int
main(int argc, char *argv[])
{
  int i;

  for(i = 1; i < argc; i++)
    printf("%s%s", argv[i], i+1 < argc ? " " : "\n");
  exit();
}
