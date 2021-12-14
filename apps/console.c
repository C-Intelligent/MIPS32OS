#include "../kernel/user/user.h"
#include "../kernel/user/types.h"
#include "../kernel/user/string.h"

void pipe_test();
void exec_test(int argc, char **argv);
void malloc_test();

int main(int argc, char **argv) {
    // printf("console main\n");
    
    exec("/bin/bash", 0, 0);
    while (1);
    
    return 0;
}

void malloc_test() {
    printf("malloc_test\n");
    char *arr = (char*)malloc(10);
    printf("malloc addr: %x\n", arr);
    safestrcpy(arr, "hello", 6);
    printf("%s\n", arr);
    while(1);
}

void exec_test(int argc, char **argv) {
    printf("exec_test\n");
    int i = 0;
    for (; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    while(1);
}

void pipe_test() {
    int fdp[2];
    pipe(fdp);
    int pid = fork();
    if (pid) {
        close(fdp[0]);
        int c = 0;
        for (;c < 200; c++) {
            write(fdp[1], "abcde ", 6);
        }
        write(fdp[1], "\nEND \n", 6);
        
        close(fdp[1]);
        printf("\nfather\n");
        while(1);
    }
    else {
        close(fdp[1]);
        char buf[10];
        buf[9] = '\0';
        char c;
        for (;; ) {
            int cc = read(fdp[0], &c, 1);
            printf("%c", c);
            if (!cc) break;
        }
        close(fdp[0]);
        printf("\nchild\n");
        while(1);
    }
}