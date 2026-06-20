
/*
 * i2c.c
 * SCL Frequency = 100kHz
 */

#define F_CPU 16000000UL

#include "I2C.h"

/*
 * Initialize I2C
 */
void I2C_Init(void)
{
    /* SCL Frequency = 100kHz */

    TWSR = 0x00;

    TWBR = 72;

    TWCR = (1 << TWEN);
}

/*
 * Generate START Condition
 */
void I2C_Start(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTA) |(1 << TWEN);
	
    while (!(TWCR & (1 << TWINT)));
}

/*
 * Generate STOP Condition
 */
void I2C_Stop(void)
{
    TWCR = (1 << TWINT) |(1 << TWSTO) |(1 << TWEN);
                     
}

/*
 * Write One Byte
 */
void I2C_Write(uint8_t data)
{
    TWDR = data;

    TWCR = (1 << TWINT) |(1 << TWEN);
           

    while (!(TWCR & (1 << TWINT)));
}

/*
 * Read Byte with ACK
 */
uint8_t I2C_ReadACK(void)
{
    TWCR = (1 << TWINT) |(1 << TWEN) |(1 << TWEA);
           
           

    while (!(TWCR & (1 << TWINT)));

    return TWDR;
}

/*
 * Read Byte with NACK
 */
uint8_t I2C_ReadNACK(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN);
          

    while (!(TWCR & (1 << TWINT)));

    return TWDR;
}