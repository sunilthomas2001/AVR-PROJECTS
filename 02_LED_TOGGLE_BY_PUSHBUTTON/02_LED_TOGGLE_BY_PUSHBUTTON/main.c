/*02_PUSHBUTTON_TOGGLE
 *Objective: Write a program to toggle between a Red LED and a Green LED using a momentary push-button switch. 
 *Requirements: The system must interface with a Red LED, a Green LED, and a push-button switch. When the switch is pressed, the program 
  must toggle the current active LED state. If the Red LED is currently ON, it must be turned OFF while the Green LED is turned ON. Conversely, 
  if the Green LED is currently ON, it must be turned OFF while the Red LED is turned ON. Each individual press of the switch should trigger
  exactly one transition between the two LEDs.
 */ 

/*
 * Push Button  -> PORTD Pin 5
 * Red LED      -> PORTD Pin 6
 * Green LED    -> PORTD Pin 7
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    // Configure I/O pins
    DDRD &= ~(1 << PD5);              // PD5 as input (Push Button)
    DDRD |= (1 << PD6);               // PD6 as output (Red LED)
    DDRD |= (1 << PD7);               // PD7 as output (Green LED)

    PORTD |= (1 << PD5);  // Enable internal pull-up resistor on PD5

    // Initial LED state
    PORTD |= (1 << PD6);              // Red LED ON
    PORTD &= ~(1 << PD7);             // Green LED OFF

    uint8_t previous_state = 1;       // Pull-up makes idle state HIGH

    while (1)
    {
        uint8_t current_state = (PIND & (1 << PD5));

        // Detect button press (HIGH -> LOW transition)
        if (!current_state && previous_state)
        {
            _delay_ms(30);            // Debounce delay

            if (!(PIND & (1 << PD5)))
            {
                PORTD ^= (1 << PD6);  // Toggle Red LED
                PORTD ^= (1 << PD7);  // Toggle Green LED
            }
        }

        previous_state = current_state;
    }
}