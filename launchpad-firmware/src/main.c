
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
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"

#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

volatile unsigned long count=0;
volatile int edge_count = 1;
volatile int clk_state;
volatile int duty_counter_a = 0;
volatile int duty_counter_b = 0;

volatile int duty_length_a = 50;
volatile int duty_length_b = 50;

int edge_counts_s[40];
int edge_counts_idx=0;
int prev_edge_count;

// An interrupt function.
void Timer1A_ISR(void);

void init_timer1() {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    //timer 1 A - regular timer, B - edge counter
	TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR | 
            TIMER_CFG_A_PERIODIC | TIMER_CFG_B_CAP_COUNT_UP);

    //prescaler 80, yields 1mhz timer clock
    TimerPrescaleSet(TIMER1_BASE, TIMER_A, 80);
	TimerLoadSet(TIMER1_BASE, TIMER_A, 1000);//1000 ovfs/s
	TimerIntRegister(TIMER1_BASE, TIMER_A, Timer1A_ISR);
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    //set PB5 as timer/edge counter pin
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB5_T1CCP1);
    GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_5);
    

	TimerEnable(TIMER1_BASE, TIMER_A | TIMER_B);
}

void init_motor() {
    //enable pin, PB6
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_6);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6, 0);

}

// main function.
int main(void) {
    int tmp;
    //80mhz system clock
	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    //enable led outputs
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);
    
    edge_count = 0;
    init_timer1();
    init_motor();
    int led_state = 0;
    while(1)
    {
        if (count > 1000 ) {
            tmp = TimerValueGet(TIMER1_BASE, TIMER_B);
            if (edge_counts_idx > 0) 
                edge_count = tmp - prev_edge_count;
            else
                edge_count = tmp;
            prev_edge_count = tmp;
            edge_counts_s[edge_counts_idx] = edge_count;
            edge_counts_idx++;
            if (edge_counts_idx == 40) 
                edge_counts_idx = 0;
            if (led_state==0)
                GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_RED);
            else
                GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);

            led_state = !led_state;
            count = 0;
        }
    }
}

// The interrupt function definition.
void Timer1A_ISR(void){
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	count++;
    if (clk_state == 0) {
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0xff);
        duty_counter_a++;
        if (duty_counter_a > duty_length_a) {
            clk_state = 1;
            duty_counter_a = 0;
        }
    } else {
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0);
        duty_counter_b++;
        if (duty_counter_b > duty_length_b) {
            clk_state = 0;
            duty_counter_b = 0;
        }
    }
}
