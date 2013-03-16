#include <avr/io.h>
#include <util/delay.h>
#include "stepper/stepper.h"
#include "uart/uart.h"

void init() {
    uart_init();
    printf("Hello");
    stepper_init();
}

int main(void) {
    init();
    stepper_move_position(-90.0, 5.0);    
    while (1) {
        
    }
}
