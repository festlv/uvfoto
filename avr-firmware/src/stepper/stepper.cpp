#include <math.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdio.h>
#include "stepper/stepper.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

const uint8_t STEP_SEQUENCE[] = {
                                0b1010, 
                                0b1000,
                                0b1001, 
                                0b0001, 
                                0b0101,
                                0b0100,
                                0b0110,
                                0b0010};

static volatile int32_t current_position_steps = 0;
static volatile int32_t target_position_steps = 0;
static volatile float target_position = 0.0;

static volatile uint8_t current_phase = 0;

// max movement speed in mm/s
static volatile float current_speed = 0.0;
static volatile float target_speed=DEFAULT_SPEED;
static volatile float current_acceleration = DEFAULT_ACCELERATION;

static const float OVF_S = F_CPU / 256.0;

static volatile uint32_t num_ovfs=0;
static volatile uint32_t next_step_ovf=0;
volatile int32_t delta_steps;
static volatile uint8_t move_to_home = 0;
static volatile uint8_t is_moving = 0;

void stepper_init() {
    STEPPER_DDR |= STEP_MASK;
    STEPPER_PORT &= ~STEP_MASK;
    HOME_POS_DDR &= ~(1<<HOME_POS_BIT);
    HOME_POS_PORT |= (1<<HOME_POS_BIT);
    
    //init timers
    TCCR0 |= (1<<CS00);//prescaler 8
    TIMSK |= (1<<TOIE0);//enable overflow interrupt 
    sei();
    stepper_set_position(0.0);    
}


static inline void stepper_set_outputs() {
    STEPPER_PORT = (STEPPER_PORT &~STEP_MASK) | \
        (STEP_SEQUENCE[current_phase] << STEP_SHIFT & STEP_MASK);
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


ISR(TIMER0_OVF_vect) {
    //this is executed about once every 16 us
    num_ovfs++;
    if ((is_moving) && num_ovfs>=next_step_ovf) {
        if (delta_steps>0 && !move_to_home) {

            stepper_step_forward();
            delta_steps--;
        } else if (delta_steps<0 || move_to_home) {
            stepper_step_backward();
            delta_steps++;
            if (!(HOME_POS_PIN & (1<<HOME_POS_BIT))) {
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
   target_speed = MAX_SPEED;
   current_acceleration = MAX_ACCELERATION; 
   is_moving = 1;
   while (is_moving) {

   }
}
