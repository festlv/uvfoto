#include "laser_data.h"
#include <math.h>
#include <stdio.h>

const float  PI=3.14159265358979f;

static float start_angle=19.0;

float radians(float a) {
    return a * PI/180;
}

float laser_get_start_angle() {
    return start_angle;
}

void laser_set_start_angle(float angle) {
    start_angle = angle;
}

void laser_load_data(uint8_t * data_in, uint8_t * data_out) {
    float tmppos, angle, offset;
    uint8_t position_printed=0; 
    //clean current data array
    
    for (int i = 0; i < ANGULAR_DATA_SIZE; i++) {
        data_out[i] = 0xff;
    }

    for (int i = 0; i < (int)ANGULAR_RESOLUTION; i++) {
        angle = start_angle + i*ANGLE_DIFF;
        tmppos = A / cos(radians(angle)) - 
                 tan(radians(angle)) * B +
                 tan(radians(90 - 2 * angle)) * C;

        offset = (IMAGE_HALF - tmppos);
        if (offset > 0 )  {
            int source_byte_num = (int)offset / 8;
            int source_bit_mask = (1 << ((int)offset % 8));


            if (data_in[source_byte_num] & source_bit_mask) {
                int target_byte_num = i / 8;
                uint8_t target_mask = (1 << (i % 8));
                data_out[target_byte_num] &= ~(target_mask);
                if (!position_printed)
                    printf("#first point position: %f, angle: %f\n", offset, angle);
                    position_printed = 1;
            }
        } 
    }
}


