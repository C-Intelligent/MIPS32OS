/**********************************************************
 * volatile  提醒编译器它后面所定义的变量随时都有可能改变，因此
 * 编译后的程序每次需要存储或读取这个变量的时候，都会直接从变量
 * 地址中读取数据。如果没有volatile关键字，则编译器可能优化读取
 * 和存储，可能暂时使用寄存器中的值，如果这个变量由别的程序更新
 * 了的话，将出现不一致的现象。
 **********************************************************/

#include "../inc/mfp_io.h"

void lab04_delay();

//------------------
// main()
//------------------
int main() {
  volatile unsigned int val = 1;

  while (1) {  
    while (val < 0x10000) {
      MFP_LEDS = val;
      val = val << 1;
      lab04_delay();
    }
    while (val > 0) {
      val = val >> 1;
      MFP_LEDS = val;
      lab04_delay();
    }
    val = 1;
  }
  return 0;
}

void lab04_delay() {
  volatile unsigned int j;

  for (j = 0; j < 200000; j++) ;	// delay 
}

void _mips_handle_exception(void* ctx, int reason) {
  MFP_LEDS = 0x8001;  // Display 0x8001 on LEDs to indicate error state
  while (1) ;
}

