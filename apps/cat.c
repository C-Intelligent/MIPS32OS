#include "../kernel/user/user.h"
#include "../kernel/user/string.h"

char buf[512];

void
cat(int fd)
{
  int n;

  while((n = read(fd, buf, sizeof(buf))) > 0)
    write(STDOUT, buf, n);
  if(n < 0){
    printf("[cat-err]: read error\n");
    exit();
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 1){
    cat(STDIN);
    exit();
  }

  for(i = 1; i < argc; i++){
    if((fd = open(argv[i], READ)) < 0){
      printf("[cat-err]: cannot open %s\n", argv[i]);
      exit();
    }
    cat(fd);
    close(fd);
  }
  exit();
}
