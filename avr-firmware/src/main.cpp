#include <avr/io.h>
#include <util/delay.h>
#include "stepper/stepper.h"
#include "uart/uart.h"
#include "gcode/gcode.h"

void init() {
    uart_init();
    printf("Hello");
    stepper_init();
}

int main(void) {
    init();
    while (1) {
        gcode_step();        
    }
}
