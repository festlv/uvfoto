#ifndef __LASER__H
#define __LASER__H


#include <stdint.h>
#include <stdbool.h>
/*
 * Part of the firmware which renders line by turning laser on at the right
 * time.
 *
 * Data is represented as bit-packed array. Each bit represents 0.1mm segment
 * of line. If bit is set, laser is on for that part of the segment, if it's
 * not set, laser is off.
 *
*/

/* Implementation details: output which pulses laser is configured as SSI (SPI)
 * interface data output. Data (line) to paint is written with laser_load_data,
 * which sets up uDMA so that SSI transfers are asynchronous.
 *
 * SSI interface bitrate is matched to speed of rotating
 * mirror, and when laser detector is triggered, uDMA is given a command to
 * start shifting out data.
 */


//Length of laser data segment, in bytes
#define LASER_DATA_LENGTH 256 

//distance from mirror to target, in 0.1mm units 
//(e.g. 1000 for 1000 * 0.1 = 100mm)
//needed to process the laser data
#define LASER_DISTANCE_TO_TARGET 820.0

//initializes timers and initial data structures 
//also sets up outputs and inputs
void laser_init();

//loads and processes data to show as a line
//length of data array has to be equal to LASER_DATA_LENGTH
void laser_load_data(uint8_t * data);

#define IMAGE_HALF 1000

#define ANGULAR_DATA_SIZE 512
#define ANGULAR_RESOLUTION 4096.0
#define START_ANGLE 17.0
#define USEFUL_ANGLE 54.0

#define A 70.0
#define B 68.0
#define C 820.0


//sets laser intensity- 0..255
void laser_set_intensity(uint8_t intensity);

//enables painting of line when start of line input is triggered (on rising
//edge)
void laser_enable();

void laser_disable();

//loads calibration data- 16mm long alternating strips of on/off
void laser_load_calibration_data();

void laser_step();

#endif
