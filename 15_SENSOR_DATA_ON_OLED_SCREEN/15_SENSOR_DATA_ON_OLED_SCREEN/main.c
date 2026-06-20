/*
 * 15_SENSOR_DATA_ON_OLED_SCREEN.c
 * Objective: Write a program to read temperature data and pressure from a BMP280 sensor and display the real-time value on an OLED screen.
 * Requirements: The system must use the I2C protocol to communicate with both the BMP280 sensor and a 128x64 OLED display. The program should 
 * first initialize both devices on the I2C bus. It must then read the raw temperature data from the BMP280, convert it into Celsius, and format the 
 * result as a string. Finally, the program must clear the OLED screen and display the current temperature in a readable format. The sensor reading and 
 * display updating should happen every 2 seconds.
 */ 
/*
 * Hardware Connections:
 *
 * Arduino Uno (ATmega328P)
 *
 * A4 (SDA) ----> BMP280 SDA
 *             -> OLED SDA
 *
 * A5 (SCL) ----> BMP280 SCL
 *             -> OLED SCL
 *
 * 5V ----------> BMP280 VCC
 *             -> OLED VCC
 *
 * GND ---------> BMP280 GND
 *             -> OLED GND
 *
 * BMP280:
 * CSB -> Not Connected
 * SDO -> Not Connected
 *
 * OLED:
 * SSD1306 I2C Display
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "I2C.h"
#include "BMP280.h"
#include "OLED.h"

int main(void)
{
    float temperature;
    float pressure;

    char tempString[20];
    char pressureString[20];

    /* Initialize I2C */
    I2C_Init();

    /* Initialize BMP280 */
    BMP280_Init();

    /* Initialize OLED */
    OLED_Init();

    while (1)
    {
        /* Read temperature in Celsius */
        temperature = BMP280_ReadTemperature();

        /* Read pressure in hPa */
        pressure = BMP280_ReadPressure();

        /* Convert float to string */
        dtostrf(temperature, 5, 2, tempString);

        dtostrf(pressure, 7, 2, pressureString);

        /* Clear OLED display */
        OLED_Clear();

        /* Display Temperature */
        OLED_SetCursor(0, 2);

        OLED_PrintString("TEMP:");

        OLED_PrintString(tempString);

        OLED_PrintString(" C");

        /* Display Pressure */
        OLED_SetCursor(2, 2);

        OLED_PrintString("PRESS:");

        OLED_PrintString(pressureString);

        /* Display update every 2 seconds */
        _delay_ms(2000);
    }
}
