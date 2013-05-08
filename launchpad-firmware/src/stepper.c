#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "stepper.h"

#include "driverlib/rom.h"
//Has the definitions for MAP_XXX function calls
//and relies on the information in rom.h to work.
#include "driverlib/rom_map.h"

#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"


#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

const uint8_t STEP_SEQUENCE[] = {
                                0xA, 
                                0x8,
                                0x9, 
                                0x1, 
                                0x5,
                                0x4,
                                0x6,
                                0x2};

static volatile int32_t current_position_steps = 0;
static volatile int32_t target_position_steps = 0;
static volatile float target_position = 0.0;

static volatile uint8_t current_phase = 0;

// max movement speed in mm/s
static volatile float current_speed = 0.0;
static volatile float target_speed=DEFAULT_SPEED;
static volatile float current_acceleration = DEFAULT_ACCELERATION;

static const float OVF_S = 80000000 / 1280.0;

static volatile uint32_t num_ovfs=0;
static volatile uint32_t next_step_ovf=0;
volatile int32_t delta_steps;
static volatile uint8_t move_to_home = 0;
static volatile uint8_t is_moving = 0;

static void timer_isr();

void stepper_init() {
    MAP_SysCtlPeripheralEnable(STEP_PORT_PERIPH);
    MAP_GPIOPinTypeGPIOOutput(STEP_PORT, STEP_GPIOS);
    
    MAP_GPIOPinTypeGPIOInput(STEP_PORT, HOME_POS_GPIO );
    MAP_GPIOPadConfigSet(STEP_PORT, HOME_POS_GPIO, 
            GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    //timer 1 - stepper clock
    MAP_TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC);
    MAP_TimerLoadSet(TIMER1_BASE, TIMER_A, 1280); //should produce 62500 ovf/s
    TimerIntRegister(TIMER1_BASE, TIMER_A, timer_isr);
    MAP_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    MAP_TimerEnable(TIMER1_BASE, TIMER_A);
    //init timers
    stepper_set_position(0.0);    
}


static inline void stepper_set_outputs() {
    MAP_GPIOPinWrite(STEP_PORT, STEP_GPIOS,
            (STEP_SEQUENCE[current_phase] << STEP_SHIFT & STEP_MASK));
}

void stepper_step_forward() {
    if (current_phase==8)
        current_phase = 0;
    stepper_set_outputs();
    current_phase++;
}

void stepper_step_backward() {
    if (current_phase==0)
        current_phase = 7;
    stepper_set_outputs();
    current_phase--;
}


static void timer_isr() {
    //this is executed about once every 16 us
    num_ovfs++;
    if ((is_moving) && num_ovfs>=next_step_ovf) {
        if (delta_steps>0 && !move_to_home) {

            stepper_step_forward();
            delta_steps--;
        } else if (delta_steps<0 || move_to_home) {
            stepper_step_backward();
            delta_steps++;
            if (!MAP_GPIOPinRead(STEP_PORT, HOME_POS_GPIO)) {
                is_moving=0;
                move_to_home=0;
            }
        } else {
            is_moving=0;
        }
        current_speed = MIN(current_speed + current_acceleration* (num_ovfs / OVF_S), 
                target_speed);
        next_step_ovf = num_ovfs + lround(OVF_S * (1/(current_speed*STEPS_PER_UNIT)));
    }
    
}


void stepper_set_position(float position) {
    current_position_steps = lround(position * STEPS_PER_UNIT);
}

void stepper_move_position(float position, float speed, float acceleration) {
    if (is_moving)
        return;
    target_position_steps = lround(position * STEPS_PER_UNIT);
    delta_steps = target_position_steps - current_position_steps;
    num_ovfs = 0;
    next_step_ovf = 0;
    current_speed = START_SPEED;
    target_speed = MIN(speed, MAX_SPEED);
    current_acceleration = MIN(acceleration, MAX_ACCELERATION); 
    
    is_moving = 1;
}

void stepper_move_position_blocking(float position, float speed,
        float acceleration) {
    stepper_move_position(position, speed, acceleration);
    while (is_moving) {

    }
}

void stepper_move_to_origin() {
   move_to_home = 1; 
   num_ovfs = 0;
   next_step_ovf = 0;
   current_speed = START_SPEED;
   target_speed = HOMING_SPEED;
   current_acceleration = MAX_ACCELERATION; 
   is_moving = 1;
   while (is_moving) {

   }
}
