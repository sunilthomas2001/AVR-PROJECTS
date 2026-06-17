/*
 * 12_ LCD_SCREEN_CONTROL.c
 *Objective: Write a program to show a 9-digit number on an LCD screen that increases with a quick button press and resets to zero with a long press.
 *Requirements: The system must use a 16x2 LCD and one push-button. On the screen, the counter should start at 000000000. When you press and let go of 
 *the button quickly, the count should go up by 1. If you hold the button down for 2 seconds or more, the counter must clear and go back to 0. The program
 *needs to use functions to check how long the button is held and to update the number on the LCD screen.
 */ 

/*
 * Hardware Connections:
* LCD Connections:
 * RS  -> PB4
 * EN  -> PB3
 * D4  -> PD5
 * D5  -> PD4
 * D6  -> PD3
 * D7  -> PD2
 * RW  -> GND
 * VSS -> GND
 * VDD -> +5V
 * V0  -> 10k Potentiometer (Contrast Control)
* Push Button:
 * PB0 -> Push Button -> GND
 *
 */


#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include "lcd.h"

/*
  Button Configuration
*/

/* Push button connected to PB0 */
#define BUTTON_PIN      PB0
#define BUTTON_PORT     PORTB
#define BUTTON_DDR      DDRB
#define BUTTON_PINREG   PINB

/* Stores the current count value */
uint32_t counter = 0;

/*          Function: updateLCD()
 Purpose:
 Display the counter as a 9-digit number with
 leading zeros.
 Example:
 1   -> 000000001
 25  -> 000000025
 123 -> 000000123
*/

void updateLCD(uint32_t value)
{
    char buffer[10];

    /* Convert number to 9-digit string */
    sprintf(buffer, "%09lu", value);

    /* Move cursor to second row, first column */
    lcd_set_cursor(1, 0);

    /* Clear second row */
    lcd_print("                ");

    /* Move cursor back to beginning */
    lcd_set_cursor(1, 0);

    /* Display the formatted counter value */
    lcd_print(buffer);
}

/*             Function: checkButtonPress()

 Purpose:
 Detect whether the button was pressed for a
 short duration or a long duration.

 Return Values:
 0 = No press detected
 1 = Short press detected
 2 = Long press detected (>= 2 seconds)
*/

uint8_t checkButtonPress(void)
{
    /* Check if button is pressed
       (active LOW because pull-up is enabled) */
    if(!(BUTTON_PINREG & (1 << BUTTON_PIN)))
    {
        uint16_t pressTime = 0;

        /* Debounce delay */
        _delay_ms(50);

        /* Verify button is still pressed */
        if(!(BUTTON_PINREG & (1 << BUTTON_PIN)))
        {
            /* Measure how long button remains pressed */
            while(!(BUTTON_PINREG & (1 << BUTTON_PIN)))
            {
                _delay_ms(10);

                /* Increase elapsed press time */
                pressTime += 10;

                /* Long press detected */
                if(pressTime >= 2000)
                {
                    /* Wait until button is released */
                    while(!(BUTTON_PINREG & (1 << BUTTON_PIN)));

                    _delay_ms(50);

                    return 2;
                }
            }

            /* Button released before 2 seconds */
            _delay_ms(50);

            return 1;
        }
    }

    return 0;
}

/*
                        Main Function
*/

int main(void)
{
    /* Configure button pin as input */
    BUTTON_DDR &= ~(1 << BUTTON_PIN);

    /* Enable internal pull-up resistor */
    BUTTON_PORT |= (1 << BUTTON_PIN);

    /* Initialize LCD */
    lcd_init();

    /* Clear LCD screen */
    lcd_clear();

    /* Display heading on first row */
    lcd_set_cursor(0, 0);
    lcd_print("Counter:");

    /* Display initial value */
    updateLCD(counter);

    while(1)
    {
        /* Check button status */
        uint8_t buttonStatus = checkButtonPress();

        /* Short press detected */
        if(buttonStatus == 1)
        {
            counter++;

            /* Keep counter within 9 digits */
            if(counter > 999999999)
            {
                counter = 0;
            }

            updateLCD(counter);
        }

        /* Long press detected */
        else if(buttonStatus == 2)
        {
            counter = 0;

            updateLCD(counter);
        }
    }
}
