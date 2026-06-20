/*
*13_UART_LED_BRIGHTNESS_CONTROL.c
 *Objective: Write a program to manage an LED’s state and intensity through a serial terminal while providing real-time status updates back to the user.
 *Requirements: The system must use UART communication at a 9600 baud rate to receive instructions. An LED must be connected to a pin capable of hardware PWM.
 *If the user sends the command ON, the LED must turn on and the system should send back the message LED is now ON. If the user sends OFF, the LED must turn off and
 *the system should reply with LED is now OFF. If the user sends a number between 0 and 100, the program must adjust the LED brightness to that percentage and reply 
 *with Brightness set to [Value]%. Student Instruction: You are responsible for selecting the most appropriate Timer (Timer0, Timer1, or Timer2) and the best 
 *PWM mode (Fast PWM or Phase Correct PWM) based on the hardware pins available and the precision required for smooth brightness control. 
 *The program must be structured using modular functions for UART communication and PWM updates.
*/
/* Hardware Connections:
LED->PD6 (OC0A) -> GND

* Baud Rate:
9600

* Commands:

ON      -> Turn LED ON
OFF     -> Turn LED OFF
0-100   -> Set LED Brightness Percentage

* Examples:
ON
OFF
25
50
75
100

## Timer Selection:

Timer0 is selected because OC0A is available on PD6,
which supports hardware PWM output.

## PWM Mode:

Fast PWM mode is selected because it provides smooth
brightness control and prevents visible LED flicker.

*/

#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//UART Configuration
#define BAUD_RATE 9600
#define UBRR_VALUE ((F_CPU / (16UL * BAUD_RATE)) - 1)


//UART Initialization

void UART_Init(void)
{
	UBRR0H = (UBRR_VALUE >> 8);
	UBRR0L = UBRR_VALUE;

	
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	

}


//Transmit One Character

void UART_TxChar(char data)
{
	while (!(UCSR0A & (1 << UDRE0)));

	
	UDR0 = data;
	

}


//Transmit String
void UART_SendString(const char *str)
{
	while (*str)
	{
		UART_TxChar(*str);
		str++;
	}
}


//Receive One Character

char UART_RxChar(void)
{
	while (!(UCSR0A & (1 << RXC0)));

	
	return UDR0;
	

}

/*
Receive String Until ENTER Key
*/
void UART_ReadString(char *buffer)
{
	uint8_t i = 0;
	char ch;

	
	while (1)
	{
		ch = UART_RxChar();

		/* Ignore leading CR/LF */
		if ((ch == '\r' || ch == '\n') && i == 0)
		{
			continue;
		}

		/* End of command */
		if (ch == '\r' || ch == '\n')
		{
			buffer[i] = '\0';
			break;
		}

		if (i < 19)
		{
			buffer[i++] = ch;
		}
	}
	

}

/*
PWM Initialization

Timer0 Fast PWM
Output Pin : PD6 (OC0A)
Prescaler  : 64

PWM Frequency ? 976 Hz
*/

void PWM_Init(void)
{
	/* Configure PD6 as output */
	DDRD |= (1 << PD6);

	
	/* Fast PWM, Non-Inverting Mode */
	TCCR0A = (1 << COM0A1) |
	(1 << WGM01)  |
	(1 << WGM00);

	/* Prescaler = 64 */
	TCCR0B = (1 << CS01) |
	(1 << CS00);

	/* LED OFF initially */
	OCR0A = 0;
	

}

/*---------------------------/*--------------------------------------
Set Brightness

Input:
0 - 100 %

Output:
0 - 255 PWM
*/

void PWM_SetBrightness(uint8_t percent)
{
	OCR0A = (percent * 255UL) / 100;
}

/*
Main Program
*/

int main(void)
{
	char command[20];
	char value[5];
	uint8_t brightness;

	
	UART_Init();
	PWM_Init();

	UART_SendString("UART LED Control Ready\r\n");

	while (1)
	{
		UART_ReadString(command);

		/* Turn LED ON */
		if (strcmp(command, "ON") == 0)
		{
			PWM_SetBrightness(100);

			UART_SendString("LED is now ON\r\n");
		}

		/* Turn LED OFF */
		else if (strcmp(command, "OFF") == 0)
		{
			PWM_SetBrightness(0);

			UART_SendString("LED is now OFF\r\n");
		}

		/* Brightness Control */
		else
		{
			brightness = atoi(command);

			if (brightness <= 100)
			{
				PWM_SetBrightness(brightness);

				UART_SendString("Brightness set to ");

				itoa(brightness, value, 10);

				UART_SendString(value);
				UART_SendString("%\r\n");
			}
			else
			{
				UART_SendString("Invalid Command\r\n");
			}
		}
	}
	

}
