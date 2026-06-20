/*
 * BMP280.h
 */ 
#ifndef BMP280_H_
#define BMP280_H_

#include <avr/io.h>

#define BMP280_ADDRESS 0x76

void BMP280_Init(void);

float BMP280_ReadTemperature(void);

float BMP280_ReadPressure(void);

#endif