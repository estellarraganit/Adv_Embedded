#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "inc/hw_gpio.h"
#include "driverlib/pin_map.h"
#include "inc/hw_types.h"


int main(void)
{
    uint32_t ui32Period;
    //timer delay variable

    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    //40MHz system clock

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
    //unlocks use of sw2
    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_DIR_MODE_IN);
    //configures pf 4 as inputs
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    //Enable GPIO peripherals and configure pins connected to LEDs
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_INT_PIN_0, GPIO_RISING_EDGE);
    IntEnable(INT_GPIOF);


    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    //Enable clock to peripheral
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    //Timer 0 as 32 bit timer in periodic mode

    //ui32Period = (SysCtlClockGet() / 10) / 2;
    ui32Period = (SysCtlClockGet() / .5);/// 50) * .75;
    //Calculate period by getting number of clock cycles div by desired frequency div and 75% duty cycle
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period-1);
    //Load period(minus 1) into Timers Interval LOAD Reg

    IntMasterEnable();
    //Enable API for all interrupts

    while(1)
    {
    }
}

void Timer0IntHandler(void)
{
    //Clear the timer interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
}

void PortEPin0IntHandler(void)
{
    //Clear the GPIO interrupt
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_0);
    
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 2);
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A);
    //Start timer and interrupts will begin triggering at timeouts
}
