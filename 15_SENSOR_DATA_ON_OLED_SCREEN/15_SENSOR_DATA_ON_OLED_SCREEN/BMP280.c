/*
 * BMP280.c
 */ 

#include "BMP280.h"
#include "I2C.h"

/* Calibration Coefficients */

uint16_t dig_T1;
int16_t  dig_T2;
int16_t  dig_T3;

uint16_t dig_P1;
int16_t  dig_P2;
int16_t  dig_P3;
int16_t  dig_P4;
int16_t  dig_P5;
int16_t  dig_P6;
int16_t  dig_P7;
int16_t  dig_P8;
int16_t  dig_P9;

int32_t t_fine;

/*
 * Write Register
 */
static void BMP280_WriteRegister(uint8_t reg,
                                 uint8_t value)
{
    I2C_Start();

    I2C_Write(BMP280_ADDRESS << 1);

    I2C_Write(reg);

    I2C_Write(value);

    I2C_Stop();
}

/*
 * Read Register
 */
static uint8_t BMP280_ReadRegister(uint8_t reg)
{
    uint8_t data;

    I2C_Start();

    I2C_Write(BMP280_ADDRESS << 1);

    I2C_Write(reg);

    I2C_Start();

    I2C_Write((BMP280_ADDRESS << 1) | 1);

    data = I2C_ReadNACK();

    I2C_Stop();

    return data;
}

/*
 * Read 16-bit Register
 */
static uint16_t BMP280_Read16(uint8_t reg)
{
    uint8_t msb;
    uint8_t lsb;

    I2C_Start();

    I2C_Write(BMP280_ADDRESS << 1);

    I2C_Write(reg);

    I2C_Start();

    I2C_Write((BMP280_ADDRESS << 1) | 1);

    lsb = I2C_ReadACK();

    msb = I2C_ReadNACK();

    I2C_Stop();

    return ((uint16_t)msb << 8) | lsb;
}

/*
 * Read Calibration Data
 */
static void BMP280_ReadCalibration(void)
{
    dig_T1 = BMP280_Read16(0x88);
    dig_T2 = (int16_t)BMP280_Read16(0x8A);
    dig_T3 = (int16_t)BMP280_Read16(0x8C);

    dig_P1 = BMP280_Read16(0x8E);
    dig_P2 = (int16_t)BMP280_Read16(0x90);
    dig_P3 = (int16_t)BMP280_Read16(0x92);
    dig_P4 = (int16_t)BMP280_Read16(0x94);
    dig_P5 = (int16_t)BMP280_Read16(0x96);
    dig_P6 = (int16_t)BMP280_Read16(0x98);
    dig_P7 = (int16_t)BMP280_Read16(0x9A);
    dig_P8 = (int16_t)BMP280_Read16(0x9C);
    dig_P9 = (int16_t)BMP280_Read16(0x9E);
}

/*
 * BMP280 Initialization
 */
void BMP280_Init(void)
{
    BMP280_WriteRegister(0xF4, 0x27);

    BMP280_WriteRegister(0xF5, 0xA0);

    BMP280_ReadCalibration();
}

/*
 * Read Raw Temperature
 */
static int32_t BMP280_ReadRawTemp(void)
{
    uint8_t msb;
    uint8_t lsb;
    uint8_t xlsb;

    msb  = BMP280_ReadRegister(0xFA);
    lsb  = BMP280_ReadRegister(0xFB);
    xlsb = BMP280_ReadRegister(0xFC);

    return ((int32_t)msb << 12) |
           ((int32_t)lsb << 4) |
           (xlsb >> 4);
}

/*
 * Read Raw Pressure
 */
static int32_t BMP280_ReadRawPressure(void)
{
    uint8_t msb;
    uint8_t lsb;
    uint8_t xlsb;

    msb  = BMP280_ReadRegister(0xF7);
    lsb  = BMP280_ReadRegister(0xF8);
    xlsb = BMP280_ReadRegister(0xF9);

    return ((int32_t)msb << 12) |
           ((int32_t)lsb << 4) |
           (xlsb >> 4);
}

/*
 * Read Temperature in Celsius
 */
float BMP280_ReadTemperature(void)
{
    int32_t adc_T;

    float temperature;

    adc_T = BMP280_ReadRawTemp();

    int32_t var1;
    int32_t var2;

    var1 = ((((adc_T >> 3) -
             ((int32_t)dig_T1 << 1))) *
             ((int32_t)dig_T2)) >> 11;

    var2 = (((((adc_T >> 4) -
             ((int32_t)dig_T1)) *
             ((adc_T >> 4) -
             ((int32_t)dig_T1))) >> 12) *
             ((int32_t)dig_T3)) >> 14;

    t_fine = var1 + var2;

    temperature = (t_fine * 5 + 128) >> 8;

    return temperature / 100.0;
}

/*
 * Read Pressure in hPa
 */
float BMP280_ReadPressure(void)
{
    int32_t adc_P;

    adc_P = BMP280_ReadRawPressure();

    BMP280_ReadTemperature();

    int64_t var1;
    int64_t var2;
    int64_t p;

    var1 = ((int64_t)t_fine) - 128000;

    var2 = var1 * var1 * (int64_t)dig_P6;

    var2 = var2 +
           ((var1 * (int64_t)dig_P5) << 17);

    var2 = var2 +
           (((int64_t)dig_P4) << 35);

    var1 = ((var1 * var1 *
           (int64_t)dig_P3) >> 8) +
           ((var1 * (int64_t)dig_P2) << 12);

    var1 = ((((int64_t)1 << 47) + var1))
           * ((int64_t)dig_P1) >> 33;

    if (var1 == 0)
    {
        return 0;
    }

    p = 1048576 - adc_P;

    p = (((p << 31) - var2) * 3125) / var1;

    var1 = (((int64_t)dig_P9) *
           (p >> 13) *
           (p >> 13)) >> 25;

    var2 = (((int64_t)dig_P8) *
           p) >> 19;

    p = ((p + var1 + var2) >> 8) +
        (((int64_t)dig_P7) << 4);

    return (float)p / 25600.0;
}