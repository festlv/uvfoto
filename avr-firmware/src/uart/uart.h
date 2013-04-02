#ifndef __UART__H
#define __UART__H
#include <stdio.h>

extern int uart_putchar(char c, FILE* f);
extern void uart_putstring(char* c);
extern int uart_getchar(FILE* f);
extern char uart_getchar_noblock();
extern void uart_init(void);

/* http://www.ermicro.com/blog/?p=325 */

#endif
