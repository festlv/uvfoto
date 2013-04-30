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
    uint8_t pd1 = 0;
    uint8_t pd2 = 0;
    uint8_t pd3 = 0;
    uint8_t pd2_dir = 0;

    while (1) {
        if (PIND & (1<<PIND1)) {
            pd1 = 1;
            if (pd3 && pd2_dir == 2) {
                for (int i=0;i<300;i++) {
                        stepper_step_backward();
                        _delay_ms(1);
                }
                pd3 = pd2_dir = 0;
            } else {
                pd2 = pd3 = pd2_dir = 0;
            }
       }
        if (PIND & (1<<PIND2)) {
            pd2 = 1;
            if (pd1)
                pd2_dir = 1;//forward
            if (pd3)
                pd2_dir = 2;//backward
            if ((pd1 && pd3) || (pd1==0 && pd3==0)) {
                pd1 = pd2 = pd3 = pd2_dir = 0;
            }
       }
        if (PIND & (1<<PIND3)) {
            pd3 = 1;
            if (pd1 && pd2_dir == 1) {
                for (int i=0;i<300;i++) {
                    stepper_step_forward();
                    _delay_ms(1);
                }
                pd1 = pd2 = pd2_dir = 0;
            } else {
                pd1 = pd2 = pd2_dir = 0;
            }
        }
    }
}
