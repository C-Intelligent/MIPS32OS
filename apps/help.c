#include "../kernel/user/user.h"

int main(int argc, char **argv) {
    printf("This is help!\n");
    int i = 0;
    for (; i < argc; i++) printf("argc: %d  argv: %s\n", i, argv[i]);
    exit();
}