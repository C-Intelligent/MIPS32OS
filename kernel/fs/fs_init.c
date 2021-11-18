#include "../inc/printf.h"
#include "../inc/types.h"
#include "../inc/defs.h"
#include "../drivers/ff.h"

FATFS fs;

void fs_init() {
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
    printf("end fs_init!\n");
}