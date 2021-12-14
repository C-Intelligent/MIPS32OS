#include "console.h"

#include "uart.h"
#include "../inc/types.h"
#include "../inc/defs.h"

#define BACKSPACE 0x100
extern struct proc *curproc;
#define C(x)  ((x)-'@')  // Control-x

static void cons_intr(int (*getc)(void));
static void cons_putc(int c);
static void consputc(int c);

/***** Serial I/O code *****/

static int serial_proc_data(void) {
    if (!get_UART_DR(get_UART_LSR())) // get when data is ready
        return -1;
    return get_UART_RBR();
}

void serial_intr(void) {
    cons_intr(serial_proc_data);
}

static void serial_putc(int c) {
    while (!get_UART_TEMT(get_UART_LSR()));
    set_UART_THR(c);
}

static void serial_init(void) {
    init_uart();
}

/***** General device-independent console code *****/
// Here we manage the console input buffer,
// where we stash characters received from the keyboard or serial port
// whenever the corresponding interrupt occurs.

#define CONSBUFSIZE 512

static struct {
    u8 buf[CONSBUFSIZE]; // buffer
    u32 rpos;            // read position
    u32 wpos;            // write position
	u32 epos;  // Edit 
	struct spinlock lock; //主要用于多核的处理
} cons;

// called by device interrupt routines to feed input characters
// into the circular console input buffer.

//控制台驱动程序 当缓冲区没有字符时启动 从串口读入字符
static void cons_intr(int (*getc)(void)) {
    int c;
	int endflag = 0;
	acquire(&cons.lock);
    while (!endflag && (c = getc()) != -1) {
		switch (c) {
		case 0:
			break;
		case '\x7f':  // Backspace
			if(cons.epos != cons.wpos){
				cons.epos--;
				consputc(BACKSPACE);
			}
			break;
		default:
			if(cons.epos - cons.rpos < CONSBUFSIZE){
				c = (c == '\r') ? '\n' : c;
				cons.buf[cons.epos++ % CONSBUFSIZE] = c;
				consputc(c);
				if(c == '\n' || c == C('D') || c == C('C')
				|| cons.epos == cons.rpos + CONSBUFSIZE){
					cons.wpos = cons.epos;
					//wakeup(&input.r);
					endflag = 1;
				}
			}
			break;
		}
    }
	release(&cons.lock);
}

//从串口缓存中读入字符
int
consoleread(char *dst, int n)
{
  u_int target;
  int c;

  target = n;
  acquire(&cons.lock); //串口输入缓冲锁
  while(n > 0){
    while(cons.rpos == cons.wpos){
      if(curproc->killed){
        release(&cons.lock);
        return -1;
      }
    //   sleep(curproc);
	  //这里主动启动串口读入程序（本应是中断驱动）
	  //这里使得中断禁用期间也能工作
	  release(&cons.lock);
  	  serial_intr();
	  acquire(&cons.lock);
    }
    c = cons.buf[cons.rpos++ % CONSBUFSIZE];
    if(c == C('D')){  // EOF
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.rpos--;
      }
      break;
    }
	if(c == C('C')){  // EOF  应该结束进程
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.rpos--;
      }
      break;
    }
    *dst++ = c;
    --n;
    if(c == '\n')
      break;
  }
  release(&cons.lock);

//   consputc('0' + (target - n));

  return target - n;
}

// return the next input character from the console, or 0 if none waiting
int cons_getc(void) {
	int c;

	// poll for any pending input characters,
	// so that this function works even when interrupts are disabled
	// (e.g., when called from the kernel monitor).
	serial_intr();

	// grab the next character from the input buffer.
	if (cons.rpos != cons.wpos) {
		c = cons.buf[cons.rpos++];
		if (cons.rpos == CONSBUFSIZE)
			cons.rpos = 0;
		return c;
	}
	return 0;
}

// output a character to the console
static void consputc(int c) {
	if(c == BACKSPACE){
		serial_putc('\b'); 
		serial_putc(' '); 
		serial_putc('\b');
	} else
    	serial_putc(c);

	if (c == '\n') {
		serial_putc('\r');
	}
}

// output a character to the console
static void cons_putc(int c) {
	if (c == '\n') {
		serial_putc(c);
		serial_putc('\r');
	} else
		serial_putc(c);
}

// initialize the console devices
void cons_init(void) {
	serial_init();

	cons.lock.locked = 0;
	cons.epos = cons.rpos = cons.wpos = 0;
}

// `High'-level console I/O.  Used by readline and cprintf.

void cputchar(int c) {
	cons_putc(c);
}

int getchar(void) {
	int c;

	while ((c = cons_getc()) == 0)
		/* do nothing */;
	return c;
}
