#include "lab_i2c_gpio.h"

/**
 * @brief: software IIC, Init the GPIO which control the SDA and SCL
 */
void IIC_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(EEPROM_IIC_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Pin = EEPROM_IIC_SCL_PIN | EEPROM_IIC_SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(EEPROM_IIC_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * @brief: CPU sends the start signal
 */
void IIC_Start(void)
{
    EEPROM_I2C_SDA_1();
    EEPROM_I2C_SCL_1();
    delay_us(5);
    EEPROM_I2C_SDA_0();
    delay_us(5);
    EEPROM_I2C_SCL_0();
    delay_us(5);
}

/**
 * @brief: CPU send the stop signal
 */
void IIC_Stop(void)
{
    EEPROM_I2C_SDA_0();
    EEPROM_I2C_SCL_1();
    delay_us(5);
    EEPROM_I2C_SDA_1();
}

/**
 * @brief: Useing IIC to send _ucByte
 * @param: uint8_t type
 */
void IIC_SendByte(uint8_t _ucByte)
{
    for(uint8_t i=0; i<8; i++)
    {
        if(_ucByte & 0x80)
            EEPROM_I2C_SDA_1();
        else
            EEPROM_I2C_SDA_0();
        delay_us(5);
        EEPROM_I2C_SCL_1();
        delay_us(5);
        EEPROM_I2C_SCL_0();
        _ucByte <<= 1;
        delay_us(5);
    }
    EEPROM_I2C_SDA_1();
}

/**
 * @brief: Useing IIC to read 8bit value
 * @retval: uint8_t type
 */
uint8_t IIC_ReadByte(void)
{
    uint8_t value = 0;
    // EEPROM_I2C_SDA_1();
    for(uint8_t i=0; i<8; i++)
    {
        value <<= 1;
        EEPROM_I2C_SCL_1();
        delay_us(5);
        if(EEPROM_I2C_SDA_READ()) value|=0x01;
        EEPROM_I2C_SCL_0();
        delay_us(5);
    }
    return value;
}

/**
 * @brief: CPU is waiting for ack that periph send
 * @retval: success : 0 && failure : 1
 */
uint8_t IIC_WaitAck(void)
{
    uint8_t res;
    EEPROM_I2C_SDA_1();
    delay_us(5);
    EEPROM_I2C_SCL_1();
    delay_us(5);
    if(EEPROM_I2C_SDA_READ())
        res = 1;
    else
        res = 0;
    EEPROM_I2C_SCL_0();
    delay_us(5);
    return res;
}

/**
 * @brief: CPU send success(0) ack to periph
 */
void IIC_Ack(void)
{
    EEPROM_I2C_SDA_0();
    delay_us(5);
    EEPROM_I2C_SCL_1();
    delay_us(5);
    EEPROM_I2C_SCL_0();
    delay_us(5);
    EEPROM_I2C_SDA_1();
}

/**
 * @brief: CPU send failure(1) ack to periph
 */
void IIC_NAck(void)
{
    EEPROM_I2C_SDA_1();
    delay_us(5);
    EEPROM_I2C_SCL_1();
    delay_us(5);
    EEPROM_I2C_SCL_0();
    delay_us(5);
}