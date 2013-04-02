#ifndef __GCODE__H
#define __GCODE__H
//supported commands
//G28 - go to home position
//G1 X%f - go to specified position
//G92 X%f - set current position (no movement, just sets the current position
//variable)
//G92 - alias of G92 X0
//
extern void gcode_step();

#endif
