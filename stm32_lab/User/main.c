#include "main.h"

extern volatile uint8_t Rx_Flag;
extern uint8_t * Process_Buf;
extern volatile uint16_t Process_Count;

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
    delay_ms(300);
    Uart_Init();
    LedInit();
    // RB_Init(&uart1_rb, uart1_rx_buf, UART_RX_BUF_SIZE);
    // IIC_GPIO_Init();
    // Basic_Tim_Init();
    // lab_i2c_erom();
    // Pwm_Led_Init();
    // printf("Hello World\n");
    // Usart_SendByte(DEBUG_USART, '1');
    printf("Hello World\n");
    LED2_ON;
    delay_ms(1000);
    LED2_OFF;



    while(1)
    {

    }


    return 0;
}
