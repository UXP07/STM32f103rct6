#ifndef IIC_EROM_H
#define IIC_EROM_H

#include "lab_i2c_gpio.h"
#include "stdio.h"

#define EEPROM_DEV_ADDR         0xA0
#define EEPROM_I2C_WR           0
#define EEPROM_I2C_RE           1
#define EEPROM_PAGE_SIZE        8

#define LAB_I2C_EROM            1
#define EEPROM_SIZE             8

uint8_t EE_CheckDevice(uint8_t _Address);
uint8_t EE_WriteBytes(uint8_t *_pWriteBuff, uint16_t _usAddress, uint16_t _usSize);
uint8_t EE_ReadBytes(uint8_t *_pReadBuff, uint16_t _usAddress, uint16_t _usSize);
uint8_t lab_i2c_erom(void);

#endif