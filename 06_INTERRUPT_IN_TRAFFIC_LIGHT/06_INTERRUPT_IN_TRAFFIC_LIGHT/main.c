/*
 * 06_INTERRUPT_IN_TRAFFIC_LIGHT.c
 *Objective: Write a program to manage a traffic light sequence that includes a high-priority interrupt designed to trigger a state change 
 *only after the current light cycle completes its minimum safe duration.
 *Requirements: The system must control three LEDs (Green, Orange, and Red) connected to PORTB. Under normal operation, 
 *the program should cycle through the Green (10s), Orange (3s), and Red (10s) states in a continuous loop. An emergency push-button must
 *be interfaced with the External Interrupt 0 (INT0) pin. When the button is pressed, the program should set a flag within the Interrupt Service Routine 
 *(ISR) rather than jumping states immediately. The main program logic must check this flag at the end of the current light's duration. If the flag is set, 
 *the system must then prioritize the Green light for a set duration before clearing the flag and resuming the normal cycle.
 *
 */ 

/*
 * Green LED  -> PB0
 * Orange LED -> PB1
 * Red LED    -> PB2
 *
 * Emergency Button -> PD2 (INT0)
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t emergency_flag = 0;

// INT0 Interrupt Service Routine
ISR(INT0_vect)
{
	emergency_flag = 1;
}

// Timer1 based 1 second delay
void Delay_1Second(void)
{
	TCCR1A = 0;
	TCCR1B = 0;

	TCNT1 = 0;
	OCR1A = 15624;

	TCCR1B |= (1 << WGM12);
	TCCR1B |= (1 << CS12) | (1 << CS10);

	while (!(TIFR1 & (1 << OCF1A)));

	TIFR1 |= (1 << OCF1A);

	TCCR1B = 0;
}

// Multiple second delay
void Delay_Seconds(uint8_t seconds)
{
	for(uint8_t i = 0; i < seconds; i++)
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

	// Configure INT0 pin (PD2) as input
	DDRD &= ~(1 << PD2);

	// Enable internal pull-up resistor
	PORTD |= (1 << PD2);

	// INT0 on falling edge
	EICRA |= (1 << ISC01);
	EICRA &= ~(1 << ISC00);

	// Enable INT0
	EIMSK |= (1 << INT0);

	// Enable global interrupts
	sei();

	while (1)
	{
		// GREEN STATE
		PORTB = (1 << PB0);
		Delay_Seconds(10);

		if (emergency_flag)
		{
			PORTB = (1 << PB0);
			Delay_Seconds(10);

			emergency_flag = 0;
			continue;
		}

		// ORANGE STATE
		PORTB = (1 << PB1);
		Delay_Seconds(3);

		if (emergency_flag)
		{
			PORTB = (1 << PB0);
			Delay_Seconds(10);

			emergency_flag = 0;
			continue;
		}

		// RED STATE
		PORTB = (1 << PB2);
		Delay_Seconds(10);

		if (emergency_flag)
		{
			PORTB = (1 << PB0);
			Delay_Seconds(10);

			emergency_flag = 0;
			continue;
		}
	}
}