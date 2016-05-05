/*
*
* Team Id: 13
* Author List: Naveen Sagar, Tapish Raniwal
* Filename: main.c
* Functions: main()
* Global Variables:var1,var2
*
*/
#include<stdint.h>
#include<stdbool.h>
#include"inc/hw_memmap.h"
#include"inc/hw_types.h"
#include"driverlib/gpio.h"
#include"driverlib/pin_map.h"
#include"driverlib/sysctl.h"
#include"driverlib/uart.h"
#include"driverlib/debug.h"
#include"driverlib/adc.h"
#include"string.h"



char var1=48, var2=48;

int main(void) {
	SysCtlClocktemp_set(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigtemp_setExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	UARTCharPut(UART0_BASE, 'T');
	UARTCharPut(UART0_BASE, 'e');
	UARTCharPut(UART0_BASE, 'x');
	UARTCharPut(UART0_BASE, 't');
	UARTCharPut(UART0_BASE, ':');
	UARTCharPut(UART0_BASE, ' ');

	uint32_t ui32ADC0Value[4];
	volatile uint32_t ui32TempAvg;
	volatile uint32_t ui32TempValueC;
	volatile uint32_t ui32TempValueF;

	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);
	char temp1, temp2;


	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

	while(1)
	{
		ADCIntClear(ADC0_BASE, 1);
		ADCProcessorTrigger(ADC0_BASE, 1);
		while(!ADCIntStatus(ADC0_BASE, 1, false))
		{
		}
		ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
		ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
		ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
		ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;

		temp1 = ui32TempValueC/10 + 48;
		temp2 = ui32TempValueC%10 + 48;


		UARTCharPut(UART0_BASE, 'C');
		UARTCharPut(UART0_BASE, 'u');
		UARTCharPut(UART0_BASE, 'r');
		UARTCharPut(UART0_BASE, 'r');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 'n');
		UARTCharPut(UART0_BASE, 't');
		UARTCharPut(UART0_BASE, ':');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, temp1);
		UARTCharPut(UART0_BASE, temp2);
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, 'C');
		UARTCharPut(UART0_BASE, ' ');

		char s_text[] = "set Temp : ";
		int s_len=strlen(s_text);
		int s;
		for(s=0;s<s_len;s++)
		{
			UARTCharPut(UART0_BASE,s_text[s]);
		}
		UARTCharPut(UART0_BASE,var1);
		UARTCharPut(UART0_BASE,var2);
		UARTCharPut(UART0_BASE,' ');

		int temp_current = (temp1-48)*10 + temp2;
		int temp_set = (var1-48)*10 + var2;

		if(temp_current < temp_set )
		{
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 8);
		}
		else
		{
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 2);
		}

		SysCtlDelay(SysCtlClockGet()/3);

		if (UARTCharsAvail(UART0_BASE))
		{
			if(UARTCharGet(UART0_BASE) == 's')
			{

				char text[] = "Enter the temperature : ";
				int len1=strlen(text);
				int i=0;
				for(; i<len1;i++)
				{
					UARTCharPut(UART0_BASE,text[i]);
				}
				var1 = UARTCharGet(UART0_BASE);
				UARTCharPut(UART0_BASE,var1);

				var2 = UARTCharGet(UART0_BASE);
				UARTCharPut(UART0_BASE,var2);

				char w_text[] = " set temperature Updated to ";
				int j;
				int len2=strlen(w_text);
				for(j=0; j<len2;j++)
				{
					UARTCharPut(UART0_BASE,w_text[j]);
				}
				UARTCharPut(UART0_BASE,var1);
				UARTCharPut(UART0_BASE,var2);
				UARTCharPut(UART0_BASE,' ');
				UARTCharPut(UART0_BASE,'C');
				UARTCharPut(UART0_BASE,' ');

			}
		}

	}

}


