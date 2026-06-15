/*
 * 07_INDEPENDENT_TIMING_TASK.c
 *
 *Objective: Write a program that manages two independent timing tasks simultaneously using the 16-bit Timer1 and the 8-bit Timer2.
 *Requirements: The system must control two different LEDs. Timer1 must be configured to toggle a Status LED on PORTB Pin 1 exactly every 2 seconds.
 *Simultaneously, Timer2 must be configured to toggle a Heartbeat LED on PORTB Pin 2 every 500 milliseconds. The program must use the Compare Match 
 *interrupt for both timers to ensure high precision. Each LED's timing logic must reside in its respective Interrupt Service Routine, allowing both 
 *sequences to run independently without interfering with each other.
 *
 */ 
/*
 * Status LED    -> PB1 (Toggle every 2 seconds)
 * Heartbeat LED -> PB2 (Toggle every 500 ms)
 */



#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t timer2_count = 0;

/* Timer1 Compare Match Interrupt
   Toggle Status LED every 2 seconds
*/
ISR(TIMER1_COMPA_vect)
{
	PORTB ^= (1 << PB1);
}

/* Timer2 Compare Match Interrupt
   Generate 500 ms using multiple compare matches
*/
ISR(TIMER2_COMPA_vect)
{
	timer2_count++;

	// Approximately 500 ms
	if (timer2_count >= 31)
	{
		PORTB ^= (1 << PB2);
		timer2_count = 0;
	}
}

int main(void)
{
	// Configure LEDs as outputs
	DDRB |= (1 << PB1);
	DDRB |= (1 << PB2);

	/* ------------ Timer1 Configuration ------------ */

	TCCR1A = 0;
	TCCR1B = 0;

	TCCR1B |= (1 << WGM12);                 // CTC Mode
	TCCR1B |= (1 << CS12) | (1 << CS10);    // Prescaler 1024

	// 16MHz / 1024 = 15625 Hz
	// 2 sec = 31250 counts
	OCR1A = 31249;

	TIMSK1 |= (1 << OCIE1A);

	/* ------------ Timer2 Configuration ------------ */

	TCCR2A = 0;
	TCCR2B = 0;

	TCCR2A |= (1 << WGM21);                 // CTC Mode

	TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
	// Prescaler = 1024

	// Interrupt every ~16 ms
	OCR2A = 249;

	TIMSK2 |= (1 << OCIE2A);

	// Enable Global Interrupts
	sei();

	while (1)
	{
		// Main loop intentionally left empty
	}
}