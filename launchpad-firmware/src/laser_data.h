#ifndef __LASER_DATA__H
#define __LASER_DATA_H

#include <stdint.h>

/* Functions exported by this file are responsible for processing incoming
 * data.
 * Incoming data is represented by bit-packed array of 256 bytes. Each bit
 * corresponds to 0.1mm, so some constants in this file are size in mm
 * multiplied by 10.
 *
 */

//Length of laser data segment, in bytes
#define LASER_DATA_LENGTH 256 

//loads and processes data to show as a line
//length of data_in array has to be equal to LASER_DATA_LENGTH
//length of data_out array has to be equal to ANGULAR_DATA_SIZE
void laser_load_data(uint8_t * data_in, uint8_t * data_out);

#define IMAGE_HALF 1024

#define ANGULAR_DATA_SIZE 512
#define ANGULAR_RESOLUTION 4096.0


float laser_get_start_angle();
void laser_set_start_angle(float angle);

#define USEFUL_ANGLE 54.0


//delta between two angle-steps
#define ANGLE_DIFF (USEFUL_ANGLE / ANGULAR_RESOLUTION)

#define LASER_SPI_FREQ 1331200

//length of mirror side
#define A 70.0

//distance from mirror center to point where laser hits mirror
#define B 68.0

//distance between point where laser hits mirror to exposed plate
#define C 820.0


#endif
