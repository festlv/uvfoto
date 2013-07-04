#include "laser_intensity.h"

#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"

static uint8_t pwm_period = 255;
static uint8_t pwm_duty_cycle = 255;


void laser_set_intensity_output(uint8_t value) {
    TimerLoadSet(TIMER2_BASE, TIMER_B, pwm_period);
    TimerMatchSet(TIMER2_BASE, TIMER_B, pwm_duty_cycle);
}


void laser_init_intensity_output() {
    //enable pin PB1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);

    //timer2 A - pwm generator for output
    TimerConfigure(TIMER2_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PWM);

    //Configure PB1 as T0CCP0- generates pwm 
    GPIOPinConfigure(GPIO_PB1_T2CCP1);
    GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_1);

    laser_set_intensity_output(255);

    TimerEnable(TIMER2_BASE, TIMER_B);
}



