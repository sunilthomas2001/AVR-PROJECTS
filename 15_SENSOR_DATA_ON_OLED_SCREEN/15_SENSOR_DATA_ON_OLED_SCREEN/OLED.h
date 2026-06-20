/*
 * OLED.h
 */ 
#ifndef OLED_H_
#define OLED_H_

#include <avr/io.h>

#define OLED_ADDRESS 0x3C

void OLED_Init(void);

void OLED_Command(uint8_t cmd);

void OLED_Data(uint8_t data);

void OLED_Clear(void);

void OLED_SetCursor(uint8_t page,
uint8_t column);

void OLED_PrintChar(char c);

void OLED_PrintString(char *str);

#endif