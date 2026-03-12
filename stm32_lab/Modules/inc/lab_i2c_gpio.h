#ifndef IIC_GPIO_H
#define IIC_GPIO_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "delay.h"


#define EEPROM_IIC_GPIO_PORT        GPIOB
#define EEPROM_IIC_GPIO_CLK         RCC_APB2Periph_GPIOB
#define EEPROM_IIC_SCL_PIN          GPIO_Pin_6
#define EEPROM_IIC_SDA_PIN          GPIO_Pin_7

#if 0
/* SCL = 1 */
#define EEPROM_I2C_SCL_1()          GPIO_SetBits(EEPROM_IIC_GPIO_PORT, EEPROM_IIC_SCL_PIN)
/* SCL = 0 */
#define EEPROM_I2C_SCL_0()          GPIO_ResetBits(EEPROM_IIC_GPIO_PORT, EEPROM_IIC_SCL_PIN)
/* SDA = 1 */
#define EEPROM_I2C_SDA_1()          GPIO_SetBits(EEPROM_IIC_GPIO_PORT, EEPROM_IIC_SDA_PIN)
/* SDA = 0 */
#define EEPROM_I2C_SDA_0()          GPIO_ResetBits(EEPROM_IIC_GPIO_PORT, EEPROM_IIC_SDA_PIN)
/* 读SDA口线状态 */
#define EEPROM_I2C_SDA_READ()       GPIO_ReadInputDataBit(EEPROM_IIC_GPIO_PORT, EEPROM_IIC_SDA_PIN)
#else
/* SCL = 1 */
#define EEPROM_I2C_SCL_1()          EEPROM_IIC_GPIO_PORT->BSRR = EEPROM_IIC_SCL_PIN
/* SCL = 0 */
#define EEPROM_I2C_SCL_0()          EEPROM_IIC_GPIO_PORT->BRR = EEPROM_IIC_SCL_PIN
/* SDA = 1 */
#define EEPROM_I2C_SDA_1()          EEPROM_IIC_GPIO_PORT->BSRR = EEPROM_IIC_SDA_PIN
/* SDA = 0 */
#define EEPROM_I2C_SDA_0()          EEPROM_IIC_GPIO_PORT->BRR = EEPROM_IIC_SDA_PIN
/* 读SDA口线状态 */
#define EEPROM_I2C_SDA_READ()       ((EEPROM_IIC_GPIO_PORT->IDR & EEPROM_IIC_SDA_PIN) != 0)
#endif


void IIC_GPIO_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_SendByte(uint8_t _ucByte);
uint8_t IIC_ReadByte(void);
uint8_t IIC_WaitAck(void);
void IIC_Ack(void);
void IIC_NAck(void);

#endif