#include "../kernel/user/user.h"
#include "../kernel/user/string.h"

void pipe_test();
void back_test();
int main(int argc, char **argv) {
    back_test();
    exit();
}

void back_test() {
  int fd = open("demo.txt", OPEN_ALWAYS | WRITE);
  int i = 0;
  for (; i< 100; i++) {
    sleep(100);
    write(fd, "back test\n", strlen("back test\n") + 1);
  }
  close(fd);
}

void pipe_test() {
  printf("This is demo!\n");
    int p[2];
    if(pipe(p) < 0)
      printf("pipe false");

    if(fork() == 0){
      //子进程1
      close(STDOUT);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      printf("Marry");
      exit();
    }
    if(fork() == 0){
      //子进程2
      close(STDIN);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      
      char buf[32];
      gets(buf, 32);
      printf("Hello %s!\n", buf);
      exit();
    }
    close(p[0]);
    close(p[1]);
    wait();
    wait();

    exit();
}
