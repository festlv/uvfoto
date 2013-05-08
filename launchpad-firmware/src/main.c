
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "driverlib/pin_map.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"
#include "driverlib/udma.h"

//systick frequency in Hz
#define SYSTICK_FREQ 100
#include "motor.h"
#include "laser.h"
#include "stepper.h"
#include "gcode.h"

#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

volatile unsigned long count=0;



// An interrupt function.
void Timer1A_ISR(void);

void systick_interrupt();

void init_systick() {
    SysTickPeriodSet(SysCtlClockGet() / SYSTICK_FREQ); 
    SysTickIntRegister(systick_interrupt);
    SysTickEnable();
}

// main function.
int main(void) {
    int led_state = 0;
    //enable led outputs
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);

    init_systick();
    motor_init();    
    motor_start();
    laser_init();
    laser_enable();
    stepper_init();
    //initialize ssi data
    laser_load_calibration_data();
    while(1)
    {
        gcode_step();
        if (count > SYSTICK_FREQ) {

                if (led_state==0)
                GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_RED);
            else
                GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);

            led_state = !led_state;
            count = 0;
        }
    }
}

void systick_interrupt(void){
	count++;
}

