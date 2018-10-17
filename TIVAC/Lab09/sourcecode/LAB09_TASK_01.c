#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#define TARGET_IS_BLIZZARD_RB1
#include "driverlib/rom.h"

// Define PI
#ifndef M_PI
#define M_PI                    3.14159265358979323846
#endif

#define SERIES_LENGTH 100 // data buffer depth
float gSeriesData[SERIES_LENGTH]; // Array of floats SERIES_LENGTH
int32_t i32DataCount = 0; // loop counter

int main(void)
{
    float fRadians; // Variable to calculate sine
    ROM_FPULazyStackingEnable(); // Turn on Lazy Stacking
    ROM_FPUEnable(); // Turn on FPU
    // Set System Clock to 50MHz
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    // Divide 2*pi by array length since a full sine wave cycle is 2*pi radians
    fRadians = ((2 * M_PI) / SERIES_LENGTH);
    // 100 sine values are calculated
    while(i32DataCount < SERIES_LENGTH)
    {
        // sine value stored in array
        gSeriesData[i32DataCount] = sinf(fRadians * i32DataCount);
        // Incrementation next value
        i32DataCount++;
    }

    while(1)
    {
    }
}