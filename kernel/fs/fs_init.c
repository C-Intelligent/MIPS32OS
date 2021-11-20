#include "../inc/printf.h"
#include "../inc/types.h"
#include "../inc/defs.h"
#include "../drivers/ff.h"
#include "../inc/file.h"

extern struct file _stdout_f_;

FATFS fs;

void fs_init() {
    /*初始化磁盘FAT驱动*/
    const TCHAR *path = "/";
    uint8_t opt = 0;
    FRESULT res = f_mount(&fs, path, opt);
    FIL fp;
    res = f_open (&fp, "/test.txt", FA_READ);
    f_close(&fp);
    // char buffer[4096];
    // u_int br = 0;
    // FRESULT res2 = f_read (&fp, buffer, 4096, &br); 
    // printf("%u\n", br);

    // f_mkdir ("test");
    /*-----------------*/

    /*初始化标准输入输出流*/
    _stdout_f_.type = FD_SERIAL;
    _stdout_f_.readable = 1;
    _stdout_f_.writable = 1;
    
    printf("end fs_init!\n");
}