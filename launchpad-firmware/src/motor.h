#ifndef __MOTOR__H
#define __MOTOR__H

//motor connections
//PB0 - /EN
//PB6 - CLK
//motor uses TIMER0A for PWM generation

void motor_start();
void motor_stop();

void motor_init();

//allows tuning of motor RPM, by passing measured revolutions per second, 
//desired revolutions per second
void motor_tune_frequency(int measured_rps, int desired_rps);
#endif
