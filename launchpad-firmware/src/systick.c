#include "systick.h"

#include <stdint.h>
#include <stdbool.h>

#include <driverlib/systick.h>
#include <driverlib/sysctl.h>

volatile unsigned long systick_count=0;

unsigned long systick_get_count() {
    return systick_count;    
}

void systick_interrupt() {
    systick_count++;
}

void systick_init() {
    SysTickPeriodSet(SysCtlClockGet() / SYSTICK_FREQ); 
    SysTickIntRegister(systick_interrupt);
    SysTickEnable();
}
