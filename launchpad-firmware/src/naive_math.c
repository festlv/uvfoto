#include "naive_math.h"

float naive_sin(float x) {
    return x / (1.0+ (x*x / (2*3.0 - x*x + (2*3.0*x*x/(4*5.0 - x*x + (4*5.0*x*x)/1.0)))));
}

float naive_atan(float x) {
    return x / (1.0+((x*x) / (3.0 + ((2.0*x)*(2.0*x) / (5.0 + ((3.0*x)*(3.0*x)) / (7.0 + (((4.0*x)*(4.0*x))/(9.0+((5.0*x)*(5.0*x))))))))));
}


