/*
 * 05_ONEWAY_TRAFFIC_LIGHT_CONTROLLER.c
 *
 *Objective: Write a program to simulate a basic one-way traffic light controller using three LEDs to represent
 *the Green, Orange, and Red signals without using inbuilt _delay() function.
 *Requirements: The system must control three LEDs connected to any three available GPIO pins on PORTB. Upon system start, 
 *the Green LED must turn ON for a duration of 10 seconds. After this period, the Green LED must turn OFF and the 
 *Orange LED must turn ON for 3 seconds. Once the Orange duration ends, it must turn OFF and the Red LED must turn ON for 10 seconds.
 *The program must be designed to continuously cycle through this Green-Orange-Red sequence indefinitely.
 *
 */ 

/*
 * Green LED  -> PB0
 * Orange LED -> PB1
 * Red LED    -> PB2
 *
 */


#define F_CPU 16000000UL

#include <avr/io.h>

// Function to generate 1 second delay using Timer1
void Delay_1Second(void)
{
	TCCR1A = 0;                  // Normal operation
	TCCR1B = 0;

	TCNT1 = 0;                   // Reset timer count
	OCR1A = 15624;               // Compare value for 1 second

	TCCR1B |= (1 << WGM12);      // CTC mode
	TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler 1024

	while (!(TIFR1 & (1 << OCF1A)))
	{
		// Wait for compare match
	}

	TIFR1 |= (1 << OCF1A);       // Clear compare match flag
	TCCR1B = 0;                  // Stop timer
}

// Delay for multiple seconds
void Delay_Seconds(uint8_t seconds)
{
	for (uint8_t i = 0; i < seconds; i++)
	{
		Delay_1Second();
	}
}

int main(void)
{
	// Configure LEDs as outputs
	DDRB |= (1 << PB0);
	DDRB |= (1 << PB1);
	DDRB |= (1 << PB2);

	while (1)
	{
		// Green LED ON for 10 seconds
		PORTB = (1 << PB0);
		Delay_Seconds(10);

		// Orange LED ON for 3 seconds
		PORTB = (1 << PB1);
		Delay_Seconds(3);

		// Red LED ON for 10 seconds
		PORTB = (1 << PB2);
		Delay_Seconds(10);
	}
}
