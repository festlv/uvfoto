#include <avr/io.h>
#include <stdio.h>

#include "uart.h"

#ifndef BAUD
#define BAUD 9600 
#endif
#include <util/setbaud.h>

FILE *uart_output = fdevopen(uart_putchar, NULL);
//FILE *uart_input = fdevopen(uart_getchar, NULL);
//FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

//FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);



/* http://www.cs.mun.ca/~rod/Winter2007/4723/notes/serial/serial.html */

void uart_init(void) {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    
#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */ 
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */    
    stdout = uart_output;
 //   stdin = uart_input;
}

int uart_putchar(char c, FILE* f) {
    UDR0 = c;
    loop_until_bit_is_set(UCSR0A, UDRE0);
    return 0;
}

int uart_getchar(FILE* f) {
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}

char uart_getchar_noblock() {
    if (UCSR0A & (1 << RXC0)) {
        return UDR0;
    }
    else
        return 0;
}
