/*
 * 04_BINARY_COUNTER.c
 *Objective: Write a program that uses four LEDs to display a 4-bit binary count triggered by a switch.
 *Requirements: Four LEDs must be connected to PORTB (Pins 0 through 3) and a push-button must be connected to PORTD Pin 2.
 *Every time the switch is pressed, the value displayed by the LEDs must increment by one in binary format (from 0000 to 1111).
 *When the count reaches 15 (all LEDs ON), the next press must reset the counter back to 0. The program should use a dedicated function 
 *to update the LED states based on the current counter variable.
 *
 */

/*
 * LED Connections
 * LED1 -> PB0
 * LED2 -> PB1
 * LED3 -> PB2
 * LED4 -> PB3
 *
 * Push Button -> PD2
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

// Function to display the counter value on LEDs
void UpdateLEDs(uint8_t count)
{
	PORTB = count;
}

int main(void)
{
	// Configure LEDs as outputs
	DDRB |= (1 << PB0);
	DDRB |= (1 << PB1);
	DDRB |= (1 << PB2);
	DDRB |= (1 << PB3);

	// Configure push button as input
	DDRD &= ~(1 << PD2);

	// Enable internal pull-up resistor
	PORTD |= (1 << PD2);

	uint8_t counter = 0;
	uint8_t previous_state = 1;

	// Display initial count (0000)
	UpdateLEDs(counter);

	while (1)
	{
		uint8_t current_state = (PIND & (1 << PD2));

		// Detect button press (HIGH to LOW)
		if (!current_state && previous_state)
		{
			_delay_ms(30);   // Debounce delay

			if (!(PIND & (1 << PD2)))
			{
				counter++;

				// Reset counter after 15
				if (counter > 15)
				{
					counter = 0;
				}

				UpdateLEDs(counter);
			}
		}

		previous_state = current_state;
	}
}


