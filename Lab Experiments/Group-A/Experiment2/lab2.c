/*
* Team Id: 13
* Author: Naveen Sagar, Tapish Raniwal

* Filename: lab2.c

* Functions:switchPinConfig, main()

* Global Variables: preled, led, count, state, sw2st

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


// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

uint8_t preled = 4;
uint8_t led = 0;

int count = 0;
int state = 0;
int sw2st =0;

/*

* Function Name: switchPinConfig()

* Input: none

* Output: none

* Description: Set PORTF Pin 0 and Pin 4 as input. Note that Pin 0 is locked.

* Example Call: switchPinConfig();

*/
void switchPinConfig(void)
{
	// Following two line removes the lock from SW2 interfaced on PORTF Pin0 -- leave this unchanged
	LOCK_F=0x4C4F434BU;
	CR_F=GPIO_PIN_0|GPIO_PIN_4;

	// GPIO PORTF Pin 0 and Pin4
	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4,GPIO_DIR_MODE_IN); // Set Pin-4 of PORT F as Input. Modifiy this to use another switch
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
}


int main(void) {
	uint32_t ui32Period;

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

	switchPinConfig();

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);


	ui32Period = (SysCtlClockGet() / 100) / 2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);

	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	TimerEnable (TIMER0_BASE, TIMER_A);

	while(1) {
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,led);
	}
}

unsigned char detectkeypress() {
	if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)) return 'p';
	else return 'u';
}

unsigned char detectkeypress2() {
	if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)) return 'p';
	else return 'u';
}

void Timer0IntHandler (void) {
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);


	if(state==0) {
		if(detectkeypress()=='p') {
			state = 1;
		}
		else if(detectkeypress()=='u') {
			state = 0;
		}
	}
	else if (state==1) {
		if(detectkeypress()=='p') {
			state=2;
			if(preled==2) {
				led = 8;
				preled = led;
			}
			else if(preled==4) {
				led = 2;
				preled = led;
			}
			else if(preled==8) {
				led = 4;
				preled = led;
			}
		}
		else if(detectkeypress()=='u') {
			state = 0;
			led = 0;
		}

	}
	else if (state==2) {
		if(detectkeypress()=='p') {
			state=2;
		}
		else if(detectkeypress()=='u') {
			state = 0;
			led = 0;
		}
	}

	if(sw2st==0) {
		if(detectkeypress2()=='p') {
			sw2st=1;
		}
		else if(detectkeypress2()=='u') {
			sw2st=0;
		}
	}
	else if (sw2st==1) {
		if(detectkeypress2()=='p') {
			sw2st=2;
			count++;
		}
		else if(detectkeypress2()=='u') {
			sw2st=0;
			//led=0;
		}

	}
	else if (sw2st==2) {
		if(detectkeypress2()=='p') {
			sw2st=2;
		}
		else if(detectkeypress2()=='u') {
			sw2st=0;
			//led=0;
		}
	}

}
