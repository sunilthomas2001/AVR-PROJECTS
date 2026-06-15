/*
 * 03_LED_TRAIN_ANIMATION.c
 *
 *Objective: Write a program to create a train animation where a single active LED appears to move across a series of connected GPIO pins.
 *Requirements: The system must control a sequence of at least eight LEDs connected to PORTB. When the program starts, only the first LED should turn ON. 
 *After a short delay, the first LED must turn OFF and the second LED must turn ON. This sequential shifting must continue until the last LED in the row is reached. 
 *Once the train reaches the end of the line, the pattern should immediately reset to the first LED and repeat the sequence indefinitely to create a continuous moving light effect.
 *
 */ 

/*
 * LED Connections
 * LED1 -> PB0
 * LED2 -> PB1
 * LED3 -> PB2
 * LED4 -> PB3
 * LED5 -> PB4
 * LED6 -> PB5
 * LED7 -> PB6
 * LED8 -> PB7
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	// Configure all PORTB pins as output
	DDRB |= (1 << PB0);
	DDRB |= (1 << PB1);
	DDRB |= (1 << PB2);
	DDRB |= (1 << PB3);
	DDRB |= (1 << PB4);
	DDRB |= (1 << PB5);
	DDRB |= (1 << PB6);
	DDRB |= (1 << PB7);

	while (1)
	{
		// Move the LED from PB0 to PB7
		for (uint8_t i = 0; i < 8; i++)
		{
			PORTB = (1 << i);   // Turn ON only one LED
			_delay_ms(200);     // Small delay to see the movement
		}
	}
}
