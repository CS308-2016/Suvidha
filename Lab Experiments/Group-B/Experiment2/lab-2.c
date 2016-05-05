/*
*	Team ID :
*	Author List : Deepak Verma , Bhupendra Singh Bhuarya
*
*	Filename : lab-2.c
*	Functions : setup(), ledPinConfig(), switchPinConfig(), switchcolor(), Interrupt handler - Timer0IntHandler(void)
*	Global Variables: state , sw2State, sw2, led, temp
*
*
*/



#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

uint8_t state=0;
uint8_t sw2state=0;
uint8_t sw2=0;
uint8_t led=0;
uint8_t temp=8;
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

void ledPinConfig(void)
{
	// Pin-1, Pin-2 and Pin-3 of PORT F set as output.
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}


void switchPinConfig(void)
{
	// Following two line removes the lock from SW2 interfaced on PORTF Pin0
	LOCK_F=0x4C4F434BU;
	CR_F=GPIO_PIN_0|GPIO_PIN_4;

	// GPIO PORTF Pin 0 and Pin4
	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4,GPIO_DIR_MODE_IN); // Set Pin-0 and Pin-4 of PORT F as Input.
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
}

int main(void)
{

	setup();
	ledPinConfig();
	switchPinConfig();

	uint32_t ui32Period;
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ui32Period = (SysCtlClockGet()/100)/ 2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);

	while(1)
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, led);
	}
}


void switchcolor()
{
	if(temp==8)led=2;
	else led = temp * 2;
	temp = led;
}

void Timer0IntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	// Read the current state of the GPIO pin and
	// write back the opposite state


	if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4))
	{

		 if (state == 0)
			{
				state =1;
			}

			else if(state==1)
			{
				state=2;
				switchcolor();
			}

			else if(state==2)
			{
				state=2;
			}
	}

	else
		{
			if(state==0)
			{
				led=0;
				state=0;
			}

			else if(state==1)
			{
				state=0;
				led=0;
			}


			else if(state ==2)
			{
				state =0;
				led=0;
			}
		}

	if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0))
		{

			 if (sw2state == 0)
				{
					sw2state=1;
				}

				else if(sw2state==1)
				{
					sw2state=2;
					sw2=sw2+1;
				}

				else if(sw2state==2)
				{
					sw2state=2;
				}
		}

		else
			{
				if(sw2state==0)
				{
					sw2state=0;
				}

				else if(sw2state==1)
				{
					sw2state=0;
				}
				else if(sw2state ==2)
				{
					sw2state =0;
				}
			}

}
