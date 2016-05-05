/*
*	Team ID :
*	Author List : Deepak Verma , Bhupendra Singh Bhuarya
*
*	Filename : lab-4.c
*	Functions : none
*	Global Variables: mode, tempp
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include"inc/hw_memmap.h"
#include"inc/hw_types.h"
#include"driverlib/debug.h"
#include"driverlib/sysctl.h"
#include"driverlib/adc.h"
#include"driverlib/gpio.h"
#include"driverlib/pin_map.h"
#include"driverlib/sysctl.h"
#include"driverlib/uart.h"

int mode=0;
int tempp;
//int settemp=25;

int main(void)
{
	uint32_t ui32ADC0Value[4];
	volatile uint32_t ui32TempAvg;
	volatile int ui32TempValueC;
	 int settemp;
	volatile uint32_t ui32TempValueF;
	SysCtlClockSet (SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable (SYSCTL_PERIPH_ADC0);
	ADCSequenceConfigure (ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure (ADC0_BASE, 1, 0, ADC_CTL_TS);
	ADCSequenceStepConfigure (ADC0_BASE, 1, 1, ADC_CTL_TS);
	ADCSequenceStepConfigure (ADC0_BASE, 1, 2, ADC_CTL_TS);
	ADCSequenceStepConfigure (ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable (ADC0_BASE, 1);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_3);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
		GPIOPinConfigure(GPIO_PA0_U0RX);
		GPIOPinConfigure(GPIO_PA1_U0TX);
		GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
		UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
		(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));


		char tempval[10] ;

		int temp=0;
		settemp = 25;

		while(1)
		{

			if(UARTCharsAvail(UART0_BASE)) // UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
			{
				if(UARTCharGet(UART0_BASE)=='s') mode=1;
			}

			ADCIntClear(ADC0_BASE, 1);
			ADCProcessorTrigger(ADC0_BASE, 1);
			while(!ADCIntStatus(ADC0_BASE, 1, false))
			{
			}

			ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
			ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
			ui32TempValueC = (1475 -((2475 * ui32TempAvg)) / 4096)/10;
			ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;

			if(mode==1)
			{
				 tempp=0;

				while(1)
				{
					if(UARTCharsAvail(UART0_BASE)) // UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
					{
						char x =UARTCharGet(UART0_BASE) ;
						if(x=='b')
						{
							if(tempp==0)break;
							settemp=tempp;
							break;
						}
						tempp *= 10;
						tempp = tempp + x - 48 ;

					}

				}

				mode=0;
			}


			if(mode == 0)
			{
				// Characters to be sent to the serial terminal
					UARTCharPut(UART0_BASE, 'C');
					UARTCharPut(UART0_BASE, 'u');
					UARTCharPut(UART0_BASE, 'r');
					UARTCharPut(UART0_BASE, 'r');
					UARTCharPut(UART0_BASE, 'e');
					UARTCharPut(UART0_BASE, 'n');
					UARTCharPut(UART0_BASE, 't');
					UARTCharPut(UART0_BASE, ' ');
					UARTCharPut(UART0_BASE, 'T');
					UARTCharPut(UART0_BASE, 'e');
					UARTCharPut(UART0_BASE, 'm');
					UARTCharPut(UART0_BASE, 'p');
					UARTCharPut(UART0_BASE, 'e');
					UARTCharPut(UART0_BASE, 'r');
					UARTCharPut(UART0_BASE, 'a');
					UARTCharPut(UART0_BASE, 't');
					UARTCharPut(UART0_BASE, 'u');
					UARTCharPut(UART0_BASE, 'r');
					UARTCharPut(UART0_BASE, 'e');
					UARTCharPut(UART0_BASE, ' ');
					UARTCharPut(UART0_BASE, '=');
					UARTCharPut(UART0_BASE, ' ');

			//sprintf(tempval , "%d", ui32TempValueC )
			temp=ui32TempValueC;

			char x;
			int i=0;
			while(temp>0)
			{
				int t;
				t=temp%10;
				x=t+'0';
				tempval[i]=x;
				i++;
				//UARTCharPut(UART0_BASE, x);

				temp=temp/10;
			}
			int k;
			for(k=i-1; k>=0;k--)
			{
				UARTCharPut(UART0_BASE , tempval[k]);
			}

			UARTCharPut(UART0_BASE , ' ');
			UARTCharPut(UART0_BASE , ' ');

			UARTCharPut(UART0_BASE , 'S');
			UARTCharPut(UART0_BASE , 'e');
			UARTCharPut(UART0_BASE , 't');
			UARTCharPut(UART0_BASE , ' ');
			UARTCharPut(UART0_BASE , 'T');
			UARTCharPut(UART0_BASE , 'e');
			UARTCharPut(UART0_BASE , 'm');
			UARTCharPut(UART0_BASE , 'p');
			UARTCharPut(UART0_BASE , ' ');
			UARTCharPut(UART0_BASE , '=');
			UARTCharPut(UART0_BASE , ' ');

			temp = settemp;
			char y;
			int j=0;
			while(temp>0)
			{
				int t;
				t = temp%10;
				y=t+'0';
				tempval[j]=y;
				j++;
				temp = temp/10;
			}


			for(k=j-1; k>=0;k--)
			{
				UARTCharPut(UART0_BASE , tempval[k]);
			}


			if(ui32TempValueC < settemp)
			{
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1); // turn on red led
				//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);   // switch off led
			}
			else if(ui32TempValueC > settemp)
			{
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3); // turn on green led
				//SysCtlDelay(SysCtlClockGet() / (1000 * 3));
				//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
			}
			else
			{
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
			}

			//UARTCharPut(UART0_BASE, tempval[0]);
			//UARTCharPut(UART0_BASE, tempval[9]);
				//if (UARTCharsAvail(UART0_BASE)) UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));

		}

			SysCtlDelay(10000);
		}
}
