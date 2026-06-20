/*
 * 14_DC_MOTOR_SPEED_CONTROL.c
 *Objective: Write a program to regulate the rotational speed of a DC motor by mapping an analog input signal to a Pulse Width Modulation (PWM) output.
 *Requirements: The system must interface a potentiometer with ADC Channel 0 and a DC motor via a motor driver connected to the OC0A pin (PORTD Pin 6). 
 *The program must configure Timer0 in Fast PWM mode to drive the motor. A dedicated function should be used to read the 10-bit analog value from the
 *potentiometer and scale it to an 8-bit value suitable for the Output Compare Register (OCR0A). The motor speed must respond linearly to the potentiometer 
 *position, reaching maximum speed at the highest voltage input and stopping at the lowest.
 */

/*
 * Hardware Connections:
 * Potentiometer Wiper -> ADC0 (PC0 / A0)
 * Motor Driver EN1    -> OC0A (PD6)
 *
 * Timer0 is configured in Fast PWM mode.
 * ADC value (0-1023) is scaled to PWM value (0-255).
 * Motor speed varies linearly with potentiometer position.
 */

#define F_CPU 16000000UL
#include <avr/io.h>


/*
 * ADC_Init
 * Initializes the ADC module.
 */
void ADC_Init(void)
{
    /* Use AVCC as reference voltage */
    ADMUX = (1 << REFS0);

    /* Enable ADC
       Prescaler = 128
       ADC Clock = 16MHz / 128 = 125kHz 
	 */
    ADCSRA = (1 << ADEN) |
             (1 << ADPS2) |
             (1 << ADPS1) |
             (1 << ADPS0);
}

/* 
  *ADC_Read
  *Reads 10-bit ADC value from selected channel
 */
uint16_t ADC_Read(uint8_t channel)
{
    /* Select ADC channel (0-7) */
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

    /* Start conversion */
    ADCSRA |= (1 << ADSC);

    /* Wait until conversion completes */
    while (ADCSRA & (1 << ADSC));

    /* Return ADC result (0-1023) */
    return ADC;
}

/*
  *PWM Initialisation 
  *Configures Timer0 Fast PWM on OC0A (PD6)
 */
void PWM_Init(void)
{
    /* Set PD6 (OC0A) as output */
    DDRD |= (1 << PD6);

    /* Fast PWM Mode
       Non-Inverting PWM
       Prescaler = 64
	 */
    TCCR0A = (1 << COM0A1) |(1 << WGM01)|(1 << WGM00);

    TCCR0B = (1 << CS01) | (1 << CS00);

            
    /* Initial duty cycle = 0% */
    OCR0A = 0;
}

/* 
 *Scale_ADC_To_PWM
 *Converts 10-bit ADC value to 8-bit PWM value
*/

uint8_t Scale_ADC_To_PWM(uint16_t adcValue)
{
    return (adcValue * 255UL) / 1023;
}

/*
 * Main Program 
 */
int main(void)
{
    uint16_t adcValue;
    uint8_t pwmValue;

    ADC_Init();
    PWM_Init();

    while (1)
    {
        /* Read potentiometer value */
        adcValue = ADC_Read(0);

        /* Scale ADC value to PWM range */
        pwmValue = Scale_ADC_To_PWM(adcValue);

        /* Update PWM duty cycle */
        OCR0A = pwmValue;
    }
}