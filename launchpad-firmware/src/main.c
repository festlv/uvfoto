
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "motor.h"
#include "laser.h"
#include "stepper.h"
#include "gcode.h"
#include "systick.h"

#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

// main function.
int main(void) {
    int led_state = 0;
    //enable led outputs
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED);

    systick_init();
    motor_init();    
    motor_start();
    SysCtlDelay(SysCtlClockGet()/3);//delay for 1 second
    laser_init();
    stepper_init();
    //initialize ssi data
    laser_calibration_set_point(150.0);
    laser_enable();
    while(1)
    {
        gcode_step();
        laser_step();
        if (systick_get_count() % SYSTICK_FREQ == 0) {

                if (led_state==0)
                GPIOPinWrite(GPIO_PORTF_BASE, LED_RED, 0xff);
                
            else
                GPIOPinWrite(GPIO_PORTF_BASE, LED_RED, 0);

            led_state = !led_state;
        }
    }
}
