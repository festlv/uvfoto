#ifndef __LASER_INTENSITY__H
#define __LASER_INTENSITY__H

#include <stdint.h>
#include <stdbool.h>
//initializes output of laser
void laser_init_intensity_output();

//sets PWM pin value based on value argument (0-255)
void laser_set_intensity_output(uint8_t value);

#endif
