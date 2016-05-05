/*
*	Team ID :
*	Author List : Deepak Verma , Bhupendra Singh Bhuarya
*
*	Filename : lab-3.c
*	Functions : Interrupt handler - Timer0IntHandler(void)
*	Global Variables: sw1state, sw2state, sw1count, sw2count, sw1pressed, mode, delay
*
*
*/

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"

uint8_t sw1state=0;
uint8_t sw2state=0;
uint8_t sw1count=0;
uint8_t sw2count=0;
uint8_t sw1pressed=0;
uint8_t mode = 0;

#define PWM_FREQUENCY 55

uint32_t delay = 100000;

void ledPinConfig(void)
{
 // Pin-1, Pin-2 and Pin-3 of PORT F set as output.
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}


int main(void)
{
//	ledPinConfig();
	volatile uint32_t ui32Load;
	volatile uint32_t ui32PWMClock;
	volatile uint8_t ui8AdjustRed, ui8AdjustGreen, ui8AdjustBlue;
	ui8AdjustRed = 254;
	ui8AdjustGreen = 2;
	ui8AdjustBlue = 2;
	uint32_t ui32Period;

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ui32Period = (SysCtlClockGet()/100)/ 2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

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
	//	PWMGenEnable(PWM1_BASE, PWM_GEN_3);

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustGreen * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);
	PWMGenEnable(PWM1_BASE, PWM_GEN_3);

	int autoLedState = 0;

	while(1)
	{

		if(mode==0)
		{
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
		else if(mode == 1)
		{
			ui8AdjustBlue = 2;
			ui8AdjustGreen = 2;
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustBlue * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustGreen * ui32Load / 1000);
			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
			{
				ui8AdjustRed--;
				if (ui8AdjustRed < 20)
				{
					ui8AdjustRed = 20;
				}
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8AdjustRed * ui32Load / 1000);
			}

			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
			{
				ui8AdjustRed++;
				if (ui8AdjustRed > 254)
				{
					ui8AdjustRed = 254;
				}
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8AdjustRed * ui32Load / 1000);
			}

			SysCtlDelay(100000);
		}
		else if(mode == 2)
		{
			ui8AdjustRed = 2;
			ui8AdjustGreen = 2;
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8AdjustRed * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustGreen * ui32Load / 1000);
			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
			{
				ui8AdjustBlue--;
				if (ui8AdjustBlue < 20)
				{
					ui8AdjustBlue = 20;
				}
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustBlue * ui32Load / 1000);
			}

			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
			{
				ui8AdjustBlue++;
				if (ui8AdjustBlue > 254)
				{
					ui8AdjustBlue = 254;
				}
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustBlue * ui32Load / 1000);
			}

			SysCtlDelay(100000);
		}
		else if(mode == 3)
		{
			ui8AdjustBlue = 2;
			ui8AdjustRed = 2;
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustBlue * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8AdjustRed * ui32Load / 1000);
			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
			{
				ui8AdjustGreen--;
				if (ui8AdjustGreen < 20)
				{
					ui8AdjustGreen = 20;
				}
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustGreen * ui32Load / 1000);
			}

			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
			{
				ui8AdjustGreen++;
				if (ui8AdjustGreen > 254)
				{
					ui8AdjustGreen = 254;
				}
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustGreen * ui32Load / 1000);
			}

			SysCtlDelay(100000);
		}
	}

}

void Timer0IntHandler(void)
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4))
	{
			if (sw1state == 0)
			{
				sw1state = 1;
			}

			else if(sw1state==1)
			{
				sw1state=2;
				sw1pressed = 1;
				if(sw2count > 100)
				{
					sw1count++;
				}
				else sw1count=0;

				delay = delay-1000;
				if(delay<1000) delay = 1000;
			}

			else if(sw1state==2)
			{
				sw1state=2;
			}
	}

	else
		{
			if(sw1state==0)
			{
				sw1state=0;
			}

			else if(sw1state==1)
			{
				sw1state=0;
			}
			else if(sw1state ==2)
			{
				sw1state =0;
				sw1pressed = 0;
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

					delay = delay+1000;
					if(delay>100000) delay = 100000;
				}

				else if(sw2state==2)
				{
					sw2state=2;
					if(sw2count < 150) sw2count++;
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
					sw2count=0;
					sw1count=0;
				}
			}

	if(sw2count > 100)      // sw2 long pressed
	{
		if(sw1count == 1)
		{
			mode = 1;

		}
		else if(sw1count == 2) mode = 2;
		else if(sw1count == 0)
		{
			if(sw1pressed) mode = 3;
		}
	}
}

