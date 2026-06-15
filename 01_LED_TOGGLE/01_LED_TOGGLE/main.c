
/*
 *01 LED TOGGLE WITH DELAY 
 
 *Objective: Write a program to toggle an LED connected to PORTD Pin 5 with specific timing intervals for the ON and OFF states.
 Requirements: The LED must be interfaced with PORTD Pin 5. Upon system start or reset, the LED must immediately turn ON and remain
 in that state for exactly 3 seconds. After the ON period, the LED must turn OFF for a duration of exactly 5 seconds. The program
 must be designed to continuously loop this 3-second ON and 5-second OFF sequence.
 */


#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRD |= (1 << PD5);    //Enable PD5 as LED Output

	while (1)
	{
		// LED ON for 3 seconds
		PORTD |= (1 << PD5);
		_delay_ms(3000);

		// LED OFF for 5 seconds
		PORTD &= ~(1 << PD5);
		_delay_ms(5000);
	}
}
