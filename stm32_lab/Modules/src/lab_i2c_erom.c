#include "lab_i2c_erom.h"

/**
 * @brief: CPU check if there has EEPROM on IIC
 * @param: EEPROM's address, in the context is 0xA0(1010 0000)
 */
uint8_t EE_CheckDevice(uint8_t _Address)
{
    uint8_t ucAck;
    IIC_Start();
    IIC_SendByte(_Address | EEPROM_I2C_WR);
    ucAck = IIC_WaitAck();
    IIC_Stop();
    return ucAck;
}

uint8_t EE_WaitStandby(void)
{
    uint32_t wait_count = 0;
    while(EE_CheckDevice(EEPROM_DEV_ADDR))
    {
        if(wait_count++ > 0xFFFF)
            return 1;
    }
    return 0;
}

uint8_t EE_WriteBytes(uint8_t *_pWriteBuff, uint16_t _usAddress, uint16_t _usSize)
{
    uint16_t i, m;
    uint16_t usAddr;
    usAddr = _usAddress;
    for(i=0; i<_usSize; i++)
    {
        if((i == 0) || ((usAddr & (EEPROM_PAGE_SIZE-1)) == 0))
        {
            IIC_Stop();
            delay_us(5);
            
            for(m=0; m<1000; m++)
            {
                IIC_Start();
                IIC_SendByte(EEPROM_DEV_ADDR | EEPROM_I2C_WR);
                if(IIC_WaitAck() == 0)
                    break;
            }

            if(m == 1000){
                IIC_Stop();
                return 1;
            }

            IIC_SendByte((uint8_t)usAddr);
            if(IIC_WaitAck()){
                IIC_Stop();
                return 1;
            }
        }

        IIC_SendByte(_pWriteBuff[i]);
        if(IIC_WaitAck()){
            IIC_Stop();
            return 1;
        }
        
        usAddr++;
    }
    IIC_Stop();
    if(EE_WaitStandby()){
        // IIC_Stop();
        return 1;
    }

    return 0;


}

uint8_t EE_ReadBytes(uint8_t *_pReadBuff, uint16_t _usAddress, uint16_t _usSize)
{
    uint16_t i, m;
    for(m=0; m<1000; m++)
    {
        IIC_Start();
        IIC_SendByte(EEPROM_DEV_ADDR | EEPROM_I2C_WR);
        if(IIC_WaitAck() == 0)
            break;
        IIC_Stop();
        delay_us(5);
    }

    if(m == 1000){
        IIC_Stop();
        return 1;
    }

    IIC_SendByte((uint8_t)_usAddress);
    if(IIC_WaitAck()){
        IIC_Stop();
        return 1;
    }

    IIC_Start();
    IIC_SendByte(EEPROM_DEV_ADDR | EEPROM_I2C_RE);
    if(IIC_WaitAck()){
        IIC_Stop();
        return 1;
    }

    for(i=0; i<_usSize; i++)
    {
        _pReadBuff[i] = IIC_ReadByte();
        if(i != _usSize-1)
            IIC_Ack();
        else
            IIC_NAck();
    }

    IIC_Stop();
    return 0;    
}

#if LAB_I2C_EROM
uint8_t lab_i2c_erom(void)
{
    uint16_t i;
    uint8_t write_buf[EEPROM_SIZE];
    uint8_t read_buf[EEPROM_SIZE];

    if(EE_CheckDevice(EEPROM_DEV_ADDR) == 1)
    {
        printf("There has no EEPROM\n");
        return 0;
    }

    for(i=0; i<EEPROM_SIZE; i++)
    {
        write_buf[i] = i;
    }

    if(EE_WriteBytes(write_buf, 0, EEPROM_SIZE)){
        printf("Write EEPROM error\n");
        return 1;
    }
    else{
        printf("Write EEPROM success\n");
    }
    delay_ms(1000);

    if(EE_ReadBytes(read_buf, 0, EEPROM_SIZE)){
        printf("Read EEPROM error\n");
        return 1;
    }
    else{
        printf("Read EEPROM success\n");
    }

    for(i=0; i<EEPROM_SIZE; i++)
    {
        printf("%d ", write_buf[i]);
    }
    printf("\n");
    for(i=0; i<EEPROM_SIZE; i++)
    {
        printf("%d ", read_buf[i]);
    }
    printf("\n");
    printf("test successful\n");
    return 0;
}

#endif