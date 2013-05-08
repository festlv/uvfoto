#include <sys/types.h>          //Needed for caddr_t
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"      //Needed for GPIO Pins/UART base
#include "inc/hw_types.h"       //Needed for SysTick Types
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

char *heap_end = 0;

extern unsigned long __heap_end__;
extern unsigned long __StackLimit;

caddr_t _sbrk(unsigned int incr)
{
    extern unsigned long __end__;
    extern unsigned long __heap_end__;
    static char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = (caddr_t)&__end__;
    }

    prev_heap_end = heap_end;

    if (heap_end + incr > (caddr_t)&__heap_end__) {
        return (caddr_t)0;
    }

    heap_end += incr;

    return (caddr_t) prev_heap_end;
}

int _close(int file)
{
    return -1;
}

int _fstat(int file)
{
    return -1;
}

int _isatty(int file)
{
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    return -1;
}

int _open(const char *name, int flags, int mode)
{
    return -1;
}

int _read(int file, char *ptr, int len)
{
    unsigned int i;
    for( i = 0; i < len; i++ ){
        ptr[i] = (char)MAP_UARTCharGet(UART0_BASE);
    }
    return len;
}

int _write(int file, char *ptr, unsigned int len)
{
    unsigned int i;
    for(i = 0; i < len; i++){
        MAP_UARTCharPut(UART0_BASE, ptr[i]);
    }
    return i;
}
