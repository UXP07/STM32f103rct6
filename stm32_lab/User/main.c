#include "main.h"


int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
    delay_ms(300);
    Uart_Init();
    LedInit();
    // IIC_GPIO_Init();
    // Basic_Tim_Init();
    // lab_i2c_erom();
    // Pwm_Led_Init();
    LED2_ON;
    delay_ms(1000);
    LED2_OFF;



    while(1)
    {
        
    }


    return 0;
}
