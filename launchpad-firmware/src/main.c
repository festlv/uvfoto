
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

#include "motor.h"


#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

volatile unsigned long count=0;
volatile unsigned int edge_count=0;
volatile unsigned int edge_counts_s[100];

volatile unsigned int timer_values[1000];
volatile unsigned int timer_value_idx=0;

#define DATA_LENGTH 256

uint8_t ssi_data[DATA_LENGTH];

uint8_t udma_control_table[1024] __attribute__((aligned(1024)));


// An interrupt function.
void Timer1A_ISR(void);

void interrupt_handler();


void init_timer1() {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    //timer 1 A - regular timer
	TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR | 
            TIMER_CFG_A_PERIODIC | TIMER_CFG_B_PERIODIC);

    //prescaler 80, yields 1mhz timer clock
    TimerPrescaleSet(TIMER1_BASE, TIMER_BOTH, 80);
	TimerLoadSet(TIMER1_BASE, TIMER_A, 10000);//100 ovfs/s
	TimerIntRegister(TIMER1_BASE, TIMER_A, Timer1A_ISR);
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    
	TimerLoadSet(TIMER1_BASE, TIMER_B, 60000);

	TimerEnable(TIMER1_BASE, TIMER_BOTH);
}
void init_interrupt() {
    //interrupt on PB5
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_5);
    GPIOIntRegister(GPIO_PORTB_BASE, interrupt_handler);
    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_RISING_EDGE);
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_5);
}
void generate_calibration_data(uint8_t *data) {
    //generates an alternating pattern of 16mm long segments
    for (int i=0;i<DATA_LENGTH;i++)
        data[i] = 0xff;
    
    int i=0;
    while (i < DATA_LENGTH) {
        for (int j=0;j<20;j++) {
            data[i+j] = 0x00;
        }
        i+= 40;
    }

}

void setup_ssi() {
    //sets up SSI to transfer data bit-by-bit (to pulse laser)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_TI,
            SSI_MODE_MASTER, 819200, 8);
    SSIEnable(SSI0_BASE);
    //set PA5 as TX for SSI
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);
    SSIDMAEnable(SSI0_BASE, SSI_DMA_TX);

}
void setup_dma() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    uDMAEnable();
    uDMAControlBaseSet(udma_control_table);    
    //channel 11, SSI0TX
    uDMAChannelAssign(UDMA_CHANNEL_SSI0TX);
    uDMAChannelControlSet(UDMA_CHANNEL_SSI0TX | UDMA_PRI_SELECT, UDMA_SIZE_8 | UDMA_ARB_4 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE);
}

void launch_dma() {
    uDMAChannelDisable(UDMA_CHANNEL_SSI0TX);
    uDMAChannelTransferSet(UDMA_CHANNEL_SSI0TX, UDMA_MODE_BASIC, ssi_data,
            (void *)(SSI0_BASE + SSI_O_DR), DATA_LENGTH); 

    uDMAChannelEnable(UDMA_CHANNEL_SSI0TX);
}
// main function.
int main(void) {
    static int edge_idx=0;
    int led_state = 0;
    //80mhz system clock
	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    //enable led outputs
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);

    init_timer1();
    motor_init();    
    motor_start();
    setup_ssi();
    setup_dma();
    init_interrupt();
    //initialize ssi data
    generate_calibration_data(ssi_data);
    while(1)
    {
        if (count > 100 ) {

            edge_counts_s[edge_idx] = edge_count;
            //motor_tune_frequency(edge_count/4, 400);
            edge_count = 0;
            edge_idx++;
            if (edge_idx>=100)
                edge_idx=0;
            

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
}

volatile static int timer_prev_value = 0;

void interrupt_handler() {
    GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_5);
    volatile int tmp = TimerValueGet(TIMER1_BASE, TIMER_B);
    if (((timer_prev_value - tmp) > 2100) || ((timer_prev_value - tmp) < 0)) {
        edge_count++;
        launch_dma();
    }
    timer_prev_value = tmp;
    timer_values[timer_value_idx] = tmp; 
    timer_value_idx++;
    if (timer_value_idx>=1000)
        timer_value_idx=0;
     
}
