#include "lab_usart_simple.h"

/**
 * @brief: configuration of NVIC
 */
void NVIC_USART_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief: Some configuration about Usart, GPIO, NVIC, and Interruption of Usart
 */
void Uart_Init(void)
{
    RCC_APB2PeriphClockCmd(DUBUG_USART_CLK|DEBUG_USART_GPIO_CLK, ENABLE);

    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEBUG_USART_TX_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(DEBUG_USART_RX_PORT, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = DEBUG_USART_BD;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(DEBUG_USART, &USART_InitStructure);
    USART_ITConfig(DEBUG_USART, USART_IT_RXNE, ENABLE);
    NVIC_USART_Configuration();
    USART_Cmd(DEBUG_USART, ENABLE);
}


/**
 * @brief: The normal usart transmits single data without DMA, 
 *         only need 'while' function to wait for TXE
 * @param  USARTx: USART1, USART2, USART3, UART4 or UART5.
 * @param  Data: the data to transmit.
 * @retval None 
 */
void Usart_SendByte(USART_TypeDef* USARTx, uint16_t Data)
{
    USART_SendData(USARTx, Data);
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}

/**
 * @brief: Send string
 * @param USARTx: USART1, USART2, USART3, UART4 or UART5.
 * @param str: The address of string
 */
void Usart_SendString(USART_TypeDef* USARTx, char* str)
{
    char* p = str;
    while(*p != '\0')
    {
        Usart_SendByte(USARTx, (uint16_t)(*p));
        p++;
    }
    while (USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);
}

void USART1_IRQHandler(void)
{
    // uint16_t ucTemp;
    if(USART_GetITStatus(DEBUG_USART, USART_IT_RXNE) != RESET){
        // ucTemp = USART_ReceiveData(DEBUG_USART);
        // USART_SendData(DEBUG_USART, ucTemp);
        LED1_TOGGLE;
        LED2_TOGGLE;
        USART_ClearITPendingBit(DEBUG_USART, USART_IT_RXNE);
    }
}

// int fputc(int ch, FILE *f)
// {
//     while(USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);
//     USART_SendData(DEBUG_USART, (uint16_t)ch);
//     return ch;
// }


#if LAB_USART_CODE


void lab_usart_send(void)
{
    delay_init();
    delay_ms(300);
    Uart_Init();
    for(int i=0; i<100; i++)
    {
        Usart_SendString(USART1, "Hello I'm 32\n");
        delay_ms(1000);
    }
}

#endif