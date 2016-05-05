/*
* Team Id: 13
* Author: Naveen Sagar, Tapish Raniwal

* Filename: main.c

* Functions:ledPinConfig(), Timer0IntHandler(), main()

* Global Variables: sw1State,sw2State,sw1Count,sw2Count

*/

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"

#define PWM_FREQUENCY 55

/* Global Variables */

// For managing state transitions
uint8_t sw1State = 0;
uint8_t sw2State = 0;

// For detecting long presses
uint8_t sw1Count = 0;
uint8_t sw2Count = 0;

// Detect if SW1 has been pressed at least once and is still pressed
uint8_t sw1Pressed = 0;

// Auto Mode, Manual Mode 1, 2 and 3. Initialized to Auto Mode
uint8_t mode = 0;

// For controlling the speed in the auto mode
uint32_t delay = 100000;

/*

* Function Name: ledPinConfig()

* Input: none

* Output: none

* Description:Pin-1 of PORT F set as output.

* Example Call: ledPinConfig();

*/
void ledPinConfig(void)
{
	// Pin-1 of PORT F set as output. Modify this to use other 2 LEDs.
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}


/*

* Function Name: Timer0IntHandler()

* Input: none

* Output: none

* Description: Handles the timer intrupt generated

* Example Call: Timer0IntHandler();

*/
void Timer0IntHandler(void) {
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	// SW1 pressed
	if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)) {
		if(sw1State==0) sw1State = 1;
		else if(sw1State==1) {
			sw1State = 2;
			sw1Pressed = 1;

			if(sw2Count>100 && sw1Count<150) ++sw1Count;
			else sw1Count = 0;

			if(mode==0) {
				delay = delay - 1000;
				if (delay<1000) delay = 1000;
			}
		}
		else if(sw1State==2) {
			sw1State = 2;

			if(sw1Pressed<150) ++sw1Pressed;
		}
	}
	else {
		if(sw1State==0) sw1State = 0;
		else if(sw1State==1) sw1State = 0;
		else if(sw1State==2) {
			sw1State = 0;

			if(sw1Pressed>100) sw1Count = 0;
			sw1Pressed = 0;
		}
	}

	// SW2 pressed
	if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)) {
		if(sw2State==0) sw2State = 1;
		else if(sw2State==1) {
			sw2State = 2;

			if(mode==0) {
				delay = delay + 1000;
				if(delay>100000) delay = 100000;
			}
		}
		else if(sw2State==2) {
			sw2State = 2;

			if(sw2Count<150) ++sw2Count;
		}
	}
	else {
		if(sw2State==0) sw2State = 0;
		else if(sw2State==1) sw2State = 0;
		else if(sw2State==2) {
			sw2State = 0;
			sw1Count = 0;
			sw2Count = 0;
			sw1Pressed = 0;
		}
	}

	// Check if SW1 and/or SW2 have been long pressed
	if(sw1Pressed>100 && sw2Count>100) mode = 3;
	else if(sw2Count>100) {
		if(sw1Count==1) mode = 1;
		else if(sw1Count==2) mode = 2;
	}
}

int main(void) {
	volatile uint32_t ui32Load;
	volatile uint32_t ui32PWMClock;

	volatile uint8_t ui8AdjustRed, ui8AdjustGreen, ui8AdjustBlue;
	ui8AdjustRed = 254;
	ui8AdjustGreen = 2;
	ui8AdjustBlue = 2;

	// Set the system clock to 40 MHz
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

	// Set the PWM Clock to 1/64 of the system clock i.e. 625 kHz
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

	// Timer related intializations, time period is 10 ms.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	uint32_t ui32Period;
	ui32Period = (SysCtlClockGet()/100)/ 2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);

	// Enable the PWM module 1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);

	// Enable the GPIO Port F
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	// PWM related initializations
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	GPIOPinConfigure(GPIO_PF1_M1PWM5);
	GPIOPinConfigure(GPIO_PF2_M1PWM6);
	GPIOPinConfigure(GPIO_PF3_M1PWM7);

	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	ui32PWMClock = SysCtlClockGet() / 64;
	ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
	PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8AdjustRed * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
	PWMGenEnable(PWM1_BASE, PWM_GEN_2);

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustBlue * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustGreen * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);
	PWMGenEnable(PWM1_BASE, PWM_GEN_3);

	// Detecting the phase while in the auto mode, RG, GB or BR.
	int autoLedState = 0;

	while(1) {
		if(mode==0) {
			if(autoLedState==0) {
				ui8AdjustRed--;
				ui8AdjustGreen++;

				if(ui8AdjustRed==2 && ui8AdjustGreen==254) autoLedState = 1;
			}
			else if(autoLedState==1) {
				ui8AdjustGreen--;
				ui8AdjustBlue++;

				if(ui8AdjustGreen==2 && ui8AdjustBlue==254) autoLedState = 2;
			}
			else if(autoLedState==2) {
				ui8AdjustBlue--;
				ui8AdjustRed++;

				if(ui8AdjustBlue==2 && ui8AdjustRed==254) autoLedState = 0;
			}

			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8AdjustRed * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustBlue * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustGreen * ui32Load / 1000);

			SysCtlDelay(delay);
		}
		else {
			if(mode==1) {
				ui8AdjustBlue = 2;
				ui8AdjustGreen = 2;

				if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00) {
					ui8AdjustRed--;
					if (ui8AdjustRed < 20) ui8AdjustRed = 20;
				}

				if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00) {
					ui8AdjustRed++;
					if (ui8AdjustRed > 254) ui8AdjustRed = 254;
				}
			}
			else if(mode==2) {
				ui8AdjustRed = 2;
				ui8AdjustGreen = 2;

				if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00) {
					ui8AdjustBlue--;
					if (ui8AdjustBlue < 20) ui8AdjustBlue = 20;
				}

				if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00) {
					ui8AdjustBlue++;
					if (ui8AdjustBlue > 254) ui8AdjustBlue = 254;
				}

			}
			else if(mode==3) {
				ui8AdjustBlue = 2;
				ui8AdjustRed = 2;

				if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00) {
					ui8AdjustGreen--;
					if (ui8AdjustGreen < 20) ui8AdjustGreen = 20;
				}

				if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00) {
					ui8AdjustGreen++;
					if (ui8AdjustGreen > 254) ui8AdjustGreen = 254;
				}
			}

			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8AdjustRed * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustBlue * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustGreen * ui32Load / 1000);

			SysCtlDelay(100000);
		}
	}
}
