#ifndef __GCODE__H
#define __GCODE__H
//supported commands
//G28 - go to home position
//G1 X%f - go to specified position
//G92 X%f - set current position (no movement, just sets the current position
//variable)
//G92 - alias of G92 X0
//
//
//M1 - draw an alternating line of calibration strips
//M2 Y%d - draw a calibration point starting at specified bit
//M3 <400 bytes> - shift out the received 400 bytes, bit by bit, for certain
//time (set by M5)
//M4 S<0-255> - set intensity of laser (value from 0-255, 0 being the lowest
//intensity)
//M5 S<0-65536> - set exposure time (in ms)
extern void gcode_step();

#endif
