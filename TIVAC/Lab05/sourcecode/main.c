#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#define TARGET_IS_BLIZZARD_RB1
#include "driverlib/rom.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/rom_map.h"

int main(void)
{
    uint32_t ui32Period;

    // Run 40MHz System Clock
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    // Enable Clock to Peripheral Timer1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    // Configure Timer 1 to Periodic Mode
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

    // Create 0.5 Sec delay
    ui32Period = SysCtlClockGet() / 2;
    TimerLoadSet(TIMER1_BASE, TIMER_A, ui32Period-1);
    // Enable Vector in Timer1A
    IntEnable(INT_TIMER1A);
    // Enable event in Timer and generate interrupt
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // Enable ADC0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ADCHardwareOversampleConfigure(ADC0_BASE, 32);

    // Enable PortF and set Pin 2 to output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

    // Use highest priority and set ADC0 and SS2
    ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_PROCESSOR, 0);

    // Sample steps 0-2 on Sequencer 2
    ADCSequenceStepConfigure(ADC0_BASE, 2, 0, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 2, 1, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 2, 2, ADC_CTL_TS);

    // Configure Interrupt flag and sample final step in Sequencer 2
    ADCSequenceStepConfigure(ADC0_BASE,2,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);

    // Enable Sequencer 2
    ADCSequenceEnable(ADC0_BASE, 2);

    // Master interrupt enable API for all interrupts
    IntMasterEnable();
    // Start timer and trigger interrupts in timeouts
    TimerEnable(TIMER1_BASE, TIMER_A);

    while(1)
    {
    }
}

void Timer1IntHandler(void)
{
    // Clear the timer interrupt
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // ADC FIFO data stored in array
    uint32_t ui32ADC0Value[4];

    // Variables for Average, Celsius and Fahrenheit Temperatures
    volatile uint32_t ui32TempAvg;
    volatile uint32_t ui32TempValueC;
    volatile uint32_t ui32TempValueF;

    // ADC conversion complete when status flag is cleared
    ADCIntClear(ADC0_BASE, 2);
    // Trigger ADC conversion
    ADCProcessorTrigger(ADC0_BASE, 2);

    // Wait for end of conversion
    while(!ADCIntStatus(ADC0_BASE, 2, false))
    {
    }
    // Copy samples available in FIFO to buffer
    ADCSequenceDataGet(ADC0_BASE, 2, ui32ADC0Value);
    // Calculate Average, Celsius and Fahrenheit temperature
    ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
    ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
    ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
    // If temperature is greater than 72 turn on Blue LED
    if(ui32TempValueF > 72)
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
    // Turn off Blue LED
    else
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
}