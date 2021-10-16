#include "vga_print.h"

void vga_print_init()
{
    cusor = 0;
    int i = 0;
    for (i = 0; i < SCREEN_MAX; ++i) {
        chMap[i] = '\0';
    }
    volatile unsigned int *p = PRINT_CTL_ADDR;
    for (i = 0; i < SCREEN_MAX; ++i) {
        (*p) = ((int)i << 8);
    }
}
bool rt_vga_write(char * v){
    while(*v!='\0'){
        vga_print_char(*v);
        v++;
    }
}
void vga_print_char(const char ch) {
    volatile unsigned int * p = PRINT_CTL_ADDR;
    if (ch == '\n') {
        int new_cusor = (cusor / SCREEN_W + 1) * SCREEN_W;
        int i = 0;
        for (i = cusor; i < new_cusor && i < SCREEN_MAX; ++i) {
            chMap[i] = '\0';
            (*p) = (int)i << 8;
        }
        cusor = new_cusor;
    } else if (ch == '\t') {
        int new_cusor = (cusor / TAB_W + 1) * TAB_W;
        int i = 0;
        for (i = cusor; i < new_cusor && i < SCREEN_MAX; ++i) {
            chMap[i] = '\0';
            (*p) = (int)i << 8;
        }
        cusor = new_cusor;
    } else if (ch == '\r') {
        cusor = (cusor / SCREEN_W) * SCREEN_W;
    } else {
        (*p) = ((int)cusor << 8) | ((int)ch);
        chMap[cusor] = ch;
        ++cusor;
    }
    if (cusor >= SCREEN_MAX) {
        int i = 0;
        for (i = 0; i < SCREEN_MAX - SCREEN_W; ++i) {
            chMap[i] = chMap[i + SCREEN_W];
        }
        for (i = SCREEN_MAX - SCREEN_W; i < SCREEN_MAX; ++i) {
            chMap[i] = '\0';
        }
        cusor = cusor - SCREEN_W;
        for (i = 0; i < SCREEN_MAX; ++i) {
            (*p) = ((int)i << 8) | ((int)chMap[i]);
        }
    }
}