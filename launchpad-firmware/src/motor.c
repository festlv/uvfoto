#include "motor.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"


//PWM period
static float motor_period = 200;

//PWM duty cycle (half of period gives 50% duty cycle)
static int motor_duty_cycle = 100;

static void motor_set_pwm() {
    TimerLoadSet(TIMER0_BASE, TIMER_A,(int)motor_period -1);
    TimerMatchSet(TIMER0_BASE, TIMER_A, motor_duty_cycle);
}

void motor_init() {
    //enable pin, PB0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0);

    motor_stop(); 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    //timer0 A - pwm generator for motor clock
    TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM);

    //Configure PB6 as T0CCP0- generates pwm 
    GPIOPinConfigure(GPIO_PB6_T0CCP0);
    GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6);
    motor_set_pwm(); 

    TimerEnable(TIMER0_BASE, TIMER_A);

}

void motor_stop() {
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, 0xff);
}

void motor_start() {
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, 0x00);
}

