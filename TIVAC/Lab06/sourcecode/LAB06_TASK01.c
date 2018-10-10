
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
        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

        ROM_GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
        ROM_GPIOPinConfigure(GPIO_PD0_M1PWM0);
       // ROM_GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
       // ROM_GPIOPinConfigure(GPIO_PF1_M1PWM5);

        //No more buttons being used.
        HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
        HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
        HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
        ROM_GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0,GPIO_DIR_MODE_IN);
        ROM_GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0,GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

       // ROM_GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
        ui32PWMClock = SysCtlClockGet() / 64;
        ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
        //ROM_PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_UP_DOWN |PWM_GEN_MODE_NO_SYNC);
        //ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);
        PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
        PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, ui32Load);

        ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
        ROM_PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);
        ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_0);

        ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui32Load * .5);
        ROM_PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
        ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_2);

        while(1)
        {

            //int i;
            //for(i=((int) ui32Load * .10);i<ui32Load;i=i+2){
              //  ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, i);
                //SysCtlDelay(2000);
            if(ROM_GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
            {
            ui8Adjust--;
            if (ui8Adjust < 41)
            {
            ui8Adjust = 41;
            }
            ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
            }
            if(ROM_GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
            {
            ui8Adjust++;
            if (ui8Adjust > 119)
            {
            ui8Adjust = 119;
            }
            ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
            }
            ROM_SysCtlDelay(100000);
            }
            //for(i=((int) ui32Load * .90);i>=0;i=i-2){
              //  ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, i);
                //SysCtlDelay(2000);
            //}
      //  }
}
