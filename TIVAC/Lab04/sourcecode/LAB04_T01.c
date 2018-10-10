#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

int main(void)
{
    uint32_t ui32Period;
    //timer delay variable

    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    //40MHz system clock

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    //Enable GPIO peripherals and configure pins connected to LEDs

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    //Enable clock to peripheral
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    //Timer 0 as 32 bit timer in periodic mode

    //ui32Period = (SysCtlClockGet() / 10) / 2;
    ui32Period = (SysCtlClockGet() / 50) * .75;
    //Calculate period by getting number of clock cycles div by desired frequency div and 75% duty cycle
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period-1);
    //Load period(minus 1) into Timers Interval LOAD Reg

    IntEnable(INT_TIMER0A);
    //Enables specific vector associated with timer0a
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //Enables a specific event within the timer to generate interrupt
    IntMasterEnable();
    //Enable API for all interrupts

    TimerEnable(TIMER0_BASE, TIMER_A);
    //Start timer and interrupts will begin triggering at timeouts

    while(1)
    {
    }
}

void Timer0IntHandler(void)
{
    //Clear the timer interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    //Read the current state of the GPIO pin
    //write back the opposite state
    if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
    }
    else
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
    }
}
