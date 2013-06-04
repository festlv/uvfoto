#include "laser.h"
#include "laser_data.h"

#include <math.h>
#include <stdio.h>

#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"
#include "driverlib/udma.h"


#include "systick.h"

uint8_t ssi_data[ANGULAR_DATA_SIZE];

static uint8_t udma_control_table[1024] __attribute__((aligned(1024)));

volatile unsigned int timer_values[1000];
volatile unsigned int timer_value_idx=0;
volatile unsigned int timer_prev_value;

static volatile uint8_t laser_enabled = 0;
static volatile uint8_t laser_intensity=255;


volatile unsigned int edge_count=0;

static void start_of_line_handler();

static void laser_launch_dma() {
    uDMAChannelDisable(UDMA_CHANNEL_SSI0TX);
    uDMAChannelTransferSet(UDMA_CHANNEL_SSI0TX, UDMA_MODE_BASIC, ssi_data,
            (void *)(SSI0_BASE + SSI_O_DR), ANGULAR_DATA_SIZE); 

    uDMAChannelEnable(UDMA_CHANNEL_SSI0TX);
}

static void init_start_of_line_interrupt() {
    //start of line interrupt on PB5
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_5);
    GPIOIntRegister(GPIO_PORTB_BASE, start_of_line_handler);
    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_RISING_EDGE);
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_5);
}

static void laser_init_pwm() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, 0xff);
}


static void laser_init_ssi() {
    //sets up SSI to transfer data bit-by-bit (to pulse laser)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_TI,
            SSI_MODE_MASTER, LASER_SPI_FREQ, 8);
    SSIEnable(SSI0_BASE);
    //set PA5 as TX for SSI
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);
    SSIDMAEnable(SSI0_BASE, SSI_DMA_TX);

}

void laser_step() {
    if (systick_get_count() % SYSTICK_FREQ ==0) {
        edge_count=0;
    }
}
static void laser_init_dma() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    uDMAEnable();
    uDMAControlBaseSet(udma_control_table);    
    //channel 11, SSI0TX
    uDMAChannelAssign(UDMA_CHANNEL_SSI0TX);
    uDMAChannelControlSet(UDMA_CHANNEL_SSI0TX | UDMA_PRI_SELECT, UDMA_SIZE_8 | UDMA_ARB_4 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE);
}

static void laser_start_timer_interrupt() {
    TimerIntClear(TIMER3_BASE, TIMER_TIMA_TIMEOUT);

    //laser_launch_dma();
}


static void laser_init_timers() {
    //timer is used to filter out false rising edges in interrupt handler
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
	TimerConfigure(TIMER3_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PERIODIC | TIMER_CFG_A_ONE_SHOT);

    //prescaler 80, yields 1mhz timer clock
    TimerPrescaleSet(TIMER3_BASE, TIMER_B, 80);
    TimerPrescaleSet(TIMER3_BASE, TIMER_A, 80);

    TimerLoadSet(TIMER3_BASE, TIMER_A, 10);
    TimerIntRegister(TIMER3_BASE, TIMER_A, laser_start_timer_interrupt);
    TimerIntEnable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
	TimerLoadSet(TIMER3_BASE, TIMER_B, 60000);

	TimerEnable(TIMER3_BASE, TIMER_B);
}


void laser_init() {
    laser_init_timers();
    init_start_of_line_interrupt();
    laser_init_ssi();
    laser_init_dma();
    laser_init_pwm();
}



void laser_set_intensity(uint8_t intensity) {
    laser_intensity = intensity;
}

void laser_enable() {
    laser_enabled = 1;
}

void laser_disable() {
    laser_enabled = 0;
}


static void start_of_line_handler() {
    GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_5);
    volatile int tmp = TimerValueGet(TIMER3_BASE, TIMER_B);
    if (((timer_prev_value - tmp) > 2100) || ((timer_prev_value - tmp) < 0)) {
        if (laser_enabled)
            laser_launch_dma();
        edge_count++;
    }
    timer_prev_value = tmp;
    timer_values[timer_value_idx] = tmp; 
    timer_value_idx++;
    if (timer_value_idx>=1000)
        timer_value_idx=0;
}

void laser_load_calibration_data() {
    //generates an alternating pattern of 16mm long segments
    uint8_t data[LASER_DATA_LENGTH];

    for (int i=0;i<LASER_DATA_LENGTH;i++)
        data[i] = 0x00;
    
    int i=0;
    while (i < LASER_DATA_LENGTH) {
        for (int j=0;j<20;j++) {
            data[i+j] = 0xff;
        }
        i+= 40;
    }
    laser_load_data(data, ssi_data);
}


void laser_calibration_point_set_position(float position) {
    uint8_t data[LASER_DATA_LENGTH];
    for (int i=0;i<LASER_DATA_LENGTH;i++)
        data[i] = 0x00;
    int start_pixel = (int)((position) * 10);
    for (int i = start_pixel; i < (start_pixel+ 10); i++) {
        int byte_num = i/8;
        uint8_t mask = (1 << (i % 8));
        data[byte_num] |= mask;
    }

    printf("#Calibration point start pixel: %d, freq: %lu\n", start_pixel, LASER_SPI_FREQ);
    laser_load_data(data, ssi_data);
}
