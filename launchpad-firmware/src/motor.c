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

static const float Kp = 0.05;
static const float Ki = 0.001;
static const float Kd = 0.0;

float outvars[100];
int errors[100];
volatile int error_idx=0;
volatile int outvars_idx=0;

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
void motor_tune_frequency(int measured_rps, int desired_rps) {
    static int previous_error = 0;
    static float dt = 0.001;
    static int integral = 0;
    int error = desired_rps - measured_rps;
    errors[error_idx] = error;
    error_idx++;
    if (error_idx>=100)
        error_idx=0;
    integral += error*dt;
    int derivative = (error - previous_error) / dt;
    float out = Kp*error;
    outvars[outvars_idx] = out;
    outvars_idx++;
    if (outvars_idx>=100)
        outvars_idx = 0;
    motor_period -= out;
    motor_duty_cycle = (int)(motor_period/2);
    motor_set_pwm();
}

