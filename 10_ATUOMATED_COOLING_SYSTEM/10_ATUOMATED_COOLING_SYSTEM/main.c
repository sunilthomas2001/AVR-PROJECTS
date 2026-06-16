/*
 * 10_ATUOMATED_COOLING_SYSTEM.c
 *Objective: Write a program to create an automated cooling system using an LM35 sensor and a DC motor with manual power control.
 *Requirements: The system must interface with an LM35 temperature sensor and a DC motor. A push-button must be used to toggle the entire system ON and OFF.
 *When the system is ON, it should read the temperature and adjust the motor speed accordingly. At lower temperatures, the motor should run at a reduced speed, and at
 *higher temperatures, the motor should automatically switch to a higher speed. The program logic for sensor reading, speed calculation, and motor control must be
 *implemented using separate functions.
*/

/*
 * Hardware Connections:
 *
 * LM35 Temperature Sensor:
 * LM35 VCC   -> +5V
 * LM35 VOUT  -> A0
 * LM35 GND   -> GND
 *
 * Push Button:
 * ---
 * PD2-> Push Button -> GND
 *
 * DC Motor:
 * ---
 * PD6 (OC0A) -> DC Motor->GND
 *---------------
* Functionality:
* ---
* * Push button toggles the cooling system ON and OFF.
* * LM35 measures temperature.
* * Motor speed is automatically adjusted using PWM.
* * Low Temperature  (<30°C)  -> Low Speed  (25%)
* * Medium Temperature (30-39°C) -> Medium Speed (50%)
* * High Temperature (>=40°C) -> High Speed (100%)
*/

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#define BUTTON PD2

// Variable to store system state
// 0 = OFF, 1 = ON
uint8_t systemState = 0;

/*
Function: ADC_Init
Purpose : Initialize ADC to read LM35 sensor
 */

void ADC_Init(void)
{
// Select AVCC (5V) as reference voltage
ADMUX = (1 << REFS0);

// Enable ADC
// ADC Clock = 16MHz / 128 = 125kHz
ADCSRA = (1 << ADEN)  |(1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0);
}

/*
Function: ReadTemperature
Purpose : Read ADC value from LM35 sensor
*/

uint16_t ReadTemperature(void)
{
// Select ADC Channel 0 (PC0)
ADMUX &= 0xF0;
// Start ADC conversion
ADCSRA |= (1 << ADSC);

// Wait until conversion completes
while (ADCSRA & (1 << ADSC));

// Return 10-bit ADC result
return ADC;

}

/*
Function: CalculateSpeed
Purpose : Convert temperature into PWM duty cycle
*/

uint8_t CalculateSpeed(uint16_t adcValue)
{
uint16_t temperature;
// Convert ADC value into temperature
// LM35 output = 10mV per °C
temperature = (adcValue * 500UL) / 1023;

// Temperature below 30°C
if (temperature < 30)
{
    return 64;      // 25% duty cycle
}

// Temperature between 30°C and 39°C
else if (temperature < 40)
{
    return 128;     // 50% duty cycle
}

// Temperature 40°C and above
else
{
    return 255;     // 100% duty cycle
}
}

/*
Function: MotorControl
Purpose : Update PWM duty cycle
*/

void MotorControl(uint8_t speed)
{
// Load duty cycle value into OCR0A
OCR0A = speed;
}

int main(void)
{
uint16_t adcValue;
uint8_t speed;


// Configure PD6 (OC0A) as PWM output
DDRD |= (1 << PD6);

// Configure PD2 as input for push button
DDRD &= ~(1 << BUTTON);

// Enable internal pull-up resistor on PD2
PORTD |= (1 << BUTTON);

// Initialize ADC
ADC_Init();

// Configure Timer0 in Fast PWM Mode
TCCR0A = (1 << COM0A1) |   // Non-inverting PWM output
         (1 << WGM01)  |   // Fast PWM mode
         (1 << WGM00);

// Set Timer0 prescaler to 64
TCCR0B = (1 << CS01) | (1 << CS00);

// Keep motor OFF initially
OCR0A = 0;

while (1)
{
    // Check if push button is pressed
    if (!(PIND & (1 << BUTTON)))
    {
        // Debounce delay
        _delay_ms(50);

        // Confirm button press
        if (!(PIND & (1 << BUTTON)))
        {
            // Toggle system state
            systemState = !systemState;

            // Wait until button is released
            while (!(PIND & (1 << BUTTON)));

            // Additional debounce delay
            _delay_ms(50);
        }
    }

    // If cooling system is ON
    if (systemState)
    {
        // Read temperature sensor
        adcValue = ReadTemperature();

        // Determine required motor speed
        speed = CalculateSpeed(adcValue);

        // Apply PWM to motor
        MotorControl(speed);
    }
    else
    {
        // Stop motor when system is OFF
        MotorControl(0);
    }
}

}