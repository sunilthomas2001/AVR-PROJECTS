/*
 * 09_PWM_LED_BRIGHTNESS_CONTROL.c
 *Objective: Write a program to control the brightness of an LED or the speed of a small DC motor using the hardware PWM capability of Timer0.
 *Requirements: The program must initialize Timer0 in Fast PWM mode with the output directed to the OC0A pin (PORTD Pin 6). The system should start with a 25%
 *duty cycle. A push-button connected to an input pin should be used to cycle the duty cycle through four levels: 25%, 50%, 75%, and 100% OFF. Each time the button is
 *pressed, the program should update the Output Compare Register (OCR0A) to the appropriate value corresponding to the selected duty cycle. The duty cycle sequence
 *should repeat continuously in a loop (i.e., after 100%, it should return to 25%). The button input should be properly debounced (either via software delay or logic) to avoid multiple unintended triggers.
 */

/*
* PWM Output : OC0A (PD6)
* Button Input: PD2
*/


 #define F_CPU 16000000UL

 #include <avr/io.h>
 #include <util/delay.h>

 #define BUTTON PD2

 uint8_t level = 0;

 // Function to update PWM duty cycle
 void updatePWM(uint8_t level)
 {
	 if(level == 0)
	 {
		 OCR0A = 64;      // 25%
	 }
	 else if(level == 1)
	 {
		 OCR0A = 128;     // 50%
	 }
	 else if(level == 2)
	 {
		 OCR0A = 191;     // 75%
	 }
	 else
	 {
		 OCR0A = 255;     // 100%
	 }
 }

 int main(void)
 {
	 // Set PD6 (OC0A) as output
	 DDRD |= (1 << PD6);

	 // Set PD2 as input
	 DDRD &= ~(1 << BUTTON);

	 // Enable internal pull-up resistor
	 PORTD |= (1 << BUTTON);

	 // Fast PWM mode
	 TCCR0A |= (1 << WGM00) | (1 << WGM01);

	 // Non-inverting mode on OC0A
	 TCCR0A |= (1 << COM0A1);

	 // Prescaler = 64
	 TCCR0B |= (1 << CS00) | (1 << CS01);

	 // Start with 25% duty cycle
	 updatePWM(level);

	 while(1)
	 {
		 // Check button press
		 if(!(PIND & (1 << BUTTON)))
		 {
			 _delay_ms(50);   // Debounce delay

			 if(!(PIND & (1 << BUTTON)))
			 {
				 level++;

				 if(level > 3)
				 {
					 level = 0;
				 }

				 updatePWM(level);

				 // Wait for button release
				 while(!(PIND & (1 << BUTTON)));

				 _delay_ms(50);
			 }
		 }
	 }
 }