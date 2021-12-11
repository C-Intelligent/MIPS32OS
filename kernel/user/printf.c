#include "user.h"
#include "print.h"
#include "types.h"
#include <stdarg.h>

void __output__(void *arg, char *s, int l)
{
	write(STDOUT, s, l);
}

void printf(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	lp_Print(__output__, 0, fmt, ap);
	va_end(ap);
}

void panic(char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	lp_Print(__output__, 0, fmt, ap);
	va_end(ap);
	while(1);
}
