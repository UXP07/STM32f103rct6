#include "lab_iwdg.h"

void IWDG_Init(uint8_t prv, uint16_t rlv)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(prv);
    IWDG_SetReload(rlv);
    IWDG_ReloadCounter();
    IWDG_Enable();
}

void IWDG_Feed(void)
{
    IWDG_ReloadCounter();
}

#if LAB_IWDG

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
    delay_ms(300);
    // Uart_Init();
    LedInit();
    // IIC_GPIO_Init();

    // Basic_Tim_Init();
    // lab_i2c_erom();
    // Pwm_Led_Init();
    if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST))
    {
        LED1_ON;
        RCC_ClearFlag();
    }
    else
    {
        LED2_ON;
        delay_ms(500);
        LED2_OFF;
    }
    KeyInit();
    IWDG_Init(IWDG_PRE, IWDG_RELOAD);


    while(1)
    {
        if(keyScan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON){
            IWDG_Feed();
            LED2_ON;
        }
    }


    return 0;
}

#endif