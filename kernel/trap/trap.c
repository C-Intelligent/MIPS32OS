#include "../inc/defs.h"

extern unsigned long exception_handlers[];
// extern void handle_int();
// extern void handle_reserved();
extern void handle_tlb();
// extern void handle_sys();
// extern void handle_mod();

void *set_except_vector(int n, void *addr);

void trap_init()
{
    int i;

    for (i = 0; i < 32; i++) {
        // set_except_vector(i, handle_reserved);
    }

    // set_except_vector(0, handle_int);
    // set_except_vector(1, handle_mod);
    set_except_vector(2, handle_tlb);


    set_except_vector(3, handle_tlb);
    // set_except_vector(8, handle_sys);
}

void *set_except_vector(int n, void *addr)
{
    unsigned long handler = (unsigned long)addr;
    unsigned long old_handler = exception_handlers[n];
    exception_handlers[n] = handler;
    return (void *)old_handler;
}

/*
0 号异常 的处理函数为 handle_int， 
1 号异常 的处理函数为 handle_mod， 
2 号异常 的处理函数为 handle_tlb， 
3 号异常 的处理函数为 handle_tlb， 
4 号 时钟 
8 号异常 的处理函数为 handle_sys，
*/
