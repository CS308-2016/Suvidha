/*
*	Team ID :
*	Author List : Deepak Verma , Bhupendra Singh Bhuarya
*
*	Filename : lab-1.c
*	Functions : setup(), ledPinConfig(), switchPinConfig()
*	Global Variables: state , sw2State
*
*
*/


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

/*
 ------ Global Variable Declaration
*/

uint8_t state=0;
uint8_t sw2Status =0 ;

/*
* Function Name: setup()
* Input: none
* Output: none
* Description: Set crystal frequency and enable GPIO Peripherals
* Example Call: setup();
*/
void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

/*
* Function Name: ledPinConfig()
* Input: none
* Output: none
* Description: Set PORTF Pin 1, Pin 2, Pin 3 as output.
* Example Call: ledPinConfig();
*/

void ledPinConfig(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); // Pin-1,Pin-2,Pin-3(red,blue,green leds) of PORT F set as output.
}

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
	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4,GPIO_DIR_MODE_IN); // Set Pin-4 and Pin-0 of PORT F as Input.
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
}


int main(void)
{
	setup();
	ledPinConfig();
	switchPinConfig();

	uint8_t ui8LED = 0;
	uint8_t temp=2;
	uint8_t ss=0;
	uint8_t ss1=0;

	while(1)
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);

		if(temp==16) temp=2;

		if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4))
		{
			ui8LED = temp;
			if(ss==0){ss++;}
		}
		else
		{
			if(ss==1){temp=temp*2; ss=0;}
			ui8LED = 0;
		}

		if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0))
		{

			//ui8LED = temp;
			if(ss1==0){ss1++;sw2Status++;}

		}
		else
		{
			if(ss1==1){ ss1=0;}
		}


	}
}
