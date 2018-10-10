#include <stdint.h>
#include <stdbool.h>
#define PART_IS_BLIZZARD_RB1
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"

#define PWM_FREQUENCY 55


int main(void)
{
    volatile uint32_t ui32Load;
    volatile uint32_t ui32PWMClock;
    volatile uint8_t ui8Adjust;
    ui8Adjust = 83;

    ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    ROM_GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    ROM_GPIOPinConfigure(GPIO_PF1_M1PWM5); //map PF1 as PWM
    ROM_GPIOPinConfigure(GPIO_PF2_M1PWM6); //map PF2 as PWM
    ROM_GPIOPinConfigure(GPIO_PF3_M1PWM7); //map PF3 as PWM

    ROM_GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
    ui32PWMClock = SysCtlClockGet() / 64;
    ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
    ROM_PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load); // set period
    ROM_PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);

    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui32Load * .1); // setinitial duty cycle
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui32Load * .1); // set initial duty cycle
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui32Load * .1); // set initial duty cycle
    ROM_PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT | PWM_OUT_6_BIT | PWM_OUT_7_BIT, true); // enable output
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_2); //enable PWM5 which is under PWN_GEN2
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_3); //enable PWM5 which is under PWN_GEN2

    while(1)
    {
        int i;// start at 10 percent duty load and go upto 90 percent
        for(i=((int) ui32Load * .10);i<ui32Load;i=i+400){
            ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, i);
            int j;
            for(j=((int) ui32Load * .10);j<ui32Load;j=j+400){
                ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, j);
                int k;
                for(k=((int) ui32Load * .10);k<ui32Load;k=k+400){
                    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, k);
                    SysCtlDelay(20);
                }
                //SysCtlDelay(2000);
            }
            SysCtlDelay(2000);
        }
        // start at 90 percent and go down to 10 percent
        for(i=((int) ui32Load * .90);i>=0;i=i-400){
            ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, i);
            int j;
            for(j=((int) ui32Load * .90);j>=0;j=j-400){
                ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, j);
                int k;
                for(k=((int) ui32Load * .90);k>=0;k=k-400){
                    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, k);
                    SysCtlDelay(20);
                }
                //SysCtlDelay(20);
            }
            SysCtlDelay(2000);
        }
    }

}