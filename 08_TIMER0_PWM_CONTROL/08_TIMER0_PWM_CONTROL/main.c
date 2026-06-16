/*
 *08_TIMER0_PWM_CONTROL.c
 *Write a program to control the brightness of an LED or the speed of a small DC motor using the hardware PWM capability of Timer0.
 *Requirements: The program must initialize Timer0 in Fast PWM mode with the output directed to the OC0A pin (PORTD Pin 6). The system 
 *should start with a 25% duty cycle. A push-button connected to an input pin should be used to cycle the duty cycle through four levels: 
 *25%, 50%, 75%, and 100% OFF. Each time the button is pressed, the program should update the Output Compare Register (OCR0A) to the next level. 
 *The transition between speeds must be handled by a dedicated function that updates the timer registers.
 */
/*
 * PWM Output : OC0A (PD6)(LED/MOTOR)
 * Button Input: PD2
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#define BUTTON PD2

uint8_t dutyState = 0;

/* Function to update PWM duty cycle */
void updatePWM(uint8_t state)
{
    switch(state)
    {
        case 0:     // 25%
            OCR0A = 64;
            break;

        case 1:     // 50%
            OCR0A = 128;
            break;

        case 2:     // 75%
            OCR0A = 191;
            break;

        case 3:     // OFF
            OCR0A = 0;
            break;
    }
}

int main(void)
{
    /* OC0A (PD6) as output */
    DDRD |= (1 << PD6);

    /* Button (PD2) as input with pull-up */
    DDRD &= ~(1 << BUTTON);
    PORTD |= (1 << BUTTON);

    /* Timer0 Fast PWM, Non-Inverting Mode */
    TCCR0A = (1 << COM0A1) |
             (1 << WGM01)  |
             (1 << WGM00);

    /* Prescaler = 64 */
    TCCR0B = (1 << CS01) | (1 << CS00);

    /* Start at 25% duty cycle */
    updatePWM(dutyState);

    while(1)
    {
        if(!(PIND & (1 << BUTTON)))
        {
            _delay_ms(50);   // debounce

            if(!(PIND & (1 << BUTTON)))
            {
                dutyState++;

                if(dutyState > 3)
                {
                    dutyState = 0;
                }

                updatePWM(dutyState);

                while(!(PIND & (1 << BUTTON)));
                _delay_ms(50);
            }
        }
    }
}