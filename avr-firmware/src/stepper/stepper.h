#ifndef __STEPPER__H
#define __STEPPER__H

#include <stdint.h>

//1.25mm per revolution, 200 steps/rev motor in half step mode = 400 steps per
//  revolution

#define STEPS_PER_UNIT  (1/1.25*400)
// acceleration in mm/s^2
#define DEFAULT_ACCELERATION 0.1
#define MAX_ACCELERATION 1.0

#define DEFAULT_SPEED 1.0 
#define MAX_SPEED 5.0 
#define START_SPEED 0.1
#define HOMING_SPEED 1.0


#define STEPPER_PORT PORTB
#define STEPPER_DDR DDRB
#define HOME_POS_PORT PORTD
#define HOME_POS_DDR DDRD
#define HOME_POS_PIN PIND
#define HOME_POS_BIT PD0


//step sequence is shifted to the left by STEP_SHIFT
#define STEP_SHIFT 0

//output pin mask
#define STEP_MASK (0b1111 << STEP_SHIFT)

extern void stepper_init(); 
extern void stepper_set_position(float position);
extern void stepper_move_position(float position, float speed=DEFAULT_SPEED,
        float acceleration=DEFAULT_ACCELERATION);

extern void stepper_move_position_blocking(float position, float speed=DEFAULT_SPEED,
        float acceleration=DEFAULT_ACCELERATION);
extern void stepper_move_to_origin();

extern void stepper_step_backward();
extern void stepper_step_forward();
#endif
