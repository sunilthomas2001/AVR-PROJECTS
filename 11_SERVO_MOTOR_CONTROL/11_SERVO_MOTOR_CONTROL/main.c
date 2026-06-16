/*
 * 11_SERVO_MOTOR_CONTROL.c
 * Write a program to control the angular position of a servo motor based on the analog input received from a potentiometer.
 *Requirements: The system must interface a potentiometer with ADC Channel 0 and a servo motor with the OC1A pin (PORTB Pin 1). The program must configure 
 *Timer1 in 16-bit Phase Correct PWM mode to generate the 50Hz signal required for servo operation. A dedicated function should read the 10-bit analog value from the 
 *potentiometer and map it to the corresponding pulse width required to move the servo from 0 to 180 degrees. The motor position must update in real-time as the potentiometer is turned.
 */
/*
Hardware Connections
----------------------------------------------------------

Potentiometer:

Outer Pin 1  -> +5V
Outer Pin 2  -> GND
Middle Pin   -> PC0 (ADC0)

Servo Motor:

Red Wire     -> +5V
Black/Brown  -> GND
Signal Wire  -> PB1 (OC1A)

System Operation
----------------------------------------------------------

1. Potentiometer voltage is read using ADC Channel 0.
2. ADC value (0-1023) is mapped to servo pulse width.
3. Timer1 generates a 50Hz PWM signal on OC1A.
4. Servo rotates between 0° and 180° according to
   potentiometer position.

==========================================================
*/

#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdint.h>

/*
Function: ADC_Init()

Purpose:
Configure ADC to read analog voltage from potentiometer.

ADC Configuration:
- Reference Voltage = AVCC (5V)
- ADC Clock = 125kHz
- Prescaler = 128
*/

void ADC_Init(void)
{
    // Select AVCC as reference voltage
    ADMUX = (1 << REFS0);

    // Enable ADC
    // Prescaler = 128
    ADCSRA = (1 << ADEN) |
             (1 << ADPS2) |
             (1 << ADPS1) |
             (1 << ADPS0);
}

/*
Function: ADC_Read()

Purpose:
Read 10-bit ADC value from selected channel.

Parameters:
channel -> ADC channel number

Returns:
10-bit ADC result (0-1023)
*/

uint16_t ADC_Read(uint8_t channel)
{
    // Select ADC channel (ADC0-ADC7)
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

    // Start ADC conversion
    ADCSRA |= (1 << ADSC);

    // Wait until conversion completes
    while (ADCSRA & (1 << ADSC));

    // Return ADC result
    return ADC;
}

/*
Function: Servo_Set_Position()
Purpose:
Convert ADC value into servo pulse width and update
PWM duty cycle.

Servo Pulse Width Range:
0°   -> 1 ms pulse
90°  -> 1.5 ms pulse
180° -> 2 ms pulse

Mapped OCR1A Range:
1000 -> 1 ms
2000 -> 2 ms
*/
void Servo_Set_Position(uint16_t adc_value)
{
    uint16_t pulse_width;

    /*
    Map ADC Value (0-1023)
    to Pulse Width (1000-2000)

    ADC = 0     -> OCR1A = 1000
    ADC = 1023  -> OCR1A = 2000
    */

    pulse_width = 1000 + ((uint32_t)adc_value * 1000) / 1023;

    // Update PWM pulse width
    OCR1A = pulse_width;
}


//Main Function

int main(void)
{
    // Configure PB1 (OC1A) as output pin
    DDRB |= (1 << PB1);

    // Initialize ADC
    ADC_Init();

    /*
    Timer1 Configuration

    PWM Mode:
    16-bit Phase Correct PWM

    PWM Output:
    OC1A (PB1)

    Prescaler:
    8

    Frequency Calculation

    F_CPU = 16 MHz

    Timer Clock
    = 16,000,000 / 8
    = 2,000,000 Hz

    Phase Correct PWM Frequency

    Frequency = Timer Clock / (2 × TOP)

    50 = 2,000,000 / (2 × TOP)

    TOP = 20,000

    Therefore:

    ICR1 = 20000

    Result:
    PWM Frequency = 50 Hz
    */

    // Non-inverting PWM output on OC1A
    TCCR1A = (1 << COM1A1) |
             (1 << WGM11);

    // Phase Correct PWM with ICR1 as TOP
    // Prescaler = 8
    TCCR1B = (1 << WGM13) |
             (1 << CS11);

    // Set TOP value for 50Hz PWM
    ICR1 = 20000;

    // Initial servo position (approximately 90°)
    OCR1A = 1500;

    while (1)
    {
        // Read potentiometer value from ADC Channel 0
        uint16_t adc_value = ADC_Read(0);

        // Update servo position according to ADC value
        Servo_Set_Position(adc_value);
    }

    return 0;
}