#ifndef __STEPPER__H
#define __STEPPER__H

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

#define STEP_PORT GPIO_PORTD_BASE
#define STEP_PORT_PERIPH SYSCTL_PERIPH_GPIOD

#define STEP_GPIOS (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)
#define HOME_POS_GPIO GPIO_PIN_6


//step sequence is shifted to the left by STEP_SHIFT
#define STEP_SHIFT 0

//output pin mask
#define STEP_MASK (0xF << STEP_SHIFT)

extern void stepper_init(); 
extern void stepper_set_position(float position);
extern void stepper_move_position(float position, float speed,
        float acceleration);

extern void stepper_move_position_blocking(float position, float speed,
        float acceleration);
extern void stepper_move_to_origin();

extern void stepper_step_backward();
extern void stepper_step_forward();
#endif
