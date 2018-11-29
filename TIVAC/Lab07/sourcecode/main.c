#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"


#define PWM_FREQUENCY 55 // Base frequency to control servo

int main(void)
{
    volatile uint32_t ui32Load; // Variable to load the count
    volatile uint32_t ui32PWMClock; // Variable to store PWM clock
    volatile uint8_t ui8Adjust; // Variable adjust position of servo
    ui8Adjust = 83; // Center position of servo at 1.51ms

    // Run CPU at 40MHz and use PWM divider 64 to run PWM clock at 625 kHz
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    // Enable PWM1
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    // Enable GPIOD for PWM output PD0
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    // Enable GPIOF to use LaunchPad buttons
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Configure PF1 as PWM output pin for module 1, PWM generator 2
    ROM_GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
    ROM_GPIOPinConfigure(GPIO_PD0_M1PWM0);

    // Unlock GPIO commit control register
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
    // Configure pins PF0 and PF4 as inputs
    ROM_GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
    // Configure the internal pull-up resistors on both pins
    ROM_GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Set PWM clock by using 64 divider with System clock
    ui32PWMClock = SysCtlClockGet() / 64;
    // Determine the count to be loaded to register
    ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
    // Set module 1 PWM generator 0 as down counter and load value
    PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, ui32Load);

    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
    // Set PWM module 1 generator 2 to output and enable to run
    ROM_PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_0);

    while(1)
    {
        // Check if SW1 is pressed
        if(ROM_GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
        {
            // Decrement to adjust 0 degrees
            ui8Adjust--;
            if (ui8Adjust < 56)
            {
                ui8Adjust = 56;
            }
            // Load value into PWM pulse width register
            ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
        }
        // Check if SW2 is pressed
        if(ROM_GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
        {
            // Increment to adjust to 90 degrees
            ui8Adjust++;
            if (ui8Adjust > 111)
            {
                ui8Adjust = 111;
            }
            // Load value into PWM pulse width register
            ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
        }
        // Set a delay to control the speed of the loop
        ROM_SysCtlDelay(100000);
    }

}
