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
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // 必须开启AFIO时钟！


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
    // USART_ITConfig(DEBUG_USART, USART_IT_IDLE, ENABLE);
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


void DEBUG_USART_IRQHandler(void)
{
    LED1_TOGGLE;
    if(USART_GetITStatus(DEBUG_USART, USART_IT_RXNE) != RESET)
    {
        uint8_t ucTmp = USART_ReceiveData(DEBUG_USART);
        // Usart_SendByte(DEBUG_USART, ucTmp);
        printf("View Back %d\n", ucTmp);
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

/******************************************************* */ 循环缓冲区例子 ****************************************************************/
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
    LED2_ON;
    delay_ms(1000);
    LED2_OFF;



    while(1)
    {
        if(Rx_Flag == 1)
        {
            for(int i=0; i<Process_Count; i++)
            {
                Usart_SendByte(DEBUG_USART, Process_Buf[i]);
            }
            Rx_Flag = 0;
        }
        Process_Buf = NULL;
        delay_ms(10);
    }


    return 0;
}

#define BUFF_SIZE       10
uint8_t Usart_Buf1[10];
uint8_t Usart_Buf2[10];
uint8_t * Active_Buf = Usart_Buf1;
uint8_t * Process_Buf = NULL;

volatile uint16_t Active_Count = 0;
volatile uint16_t Process_Count = 0;
volatile uint8_t Rx_Flag = 0;


void USART1_IRQHandler(void)
{
    // LED1_TOGGLE;
        
    if(USART_GetITStatus(DEBUG_USART, USART_IT_RXNE) != RESET){
        if(Active_Count < BUFF_SIZE)
        {
            Active_Buf[Active_Count++] = USART_ReceiveData(DEBUG_USART);
        }
        else
        {
            USART_ReceiveData(DEBUG_USART);
        }
        USART_ClearITPendingBit(DEBUG_USART, USART_IT_RXNE);
    }
    if(USART_GetITStatus(DEBUG_USART, USART_IT_IDLE) != RESET && Process_Buf == NULL)
    {
        Process_Buf = Active_Buf;
        Process_Count = Active_Count;
        Rx_Flag = 1;
        if(Active_Buf == Usart_Buf1)
        {
            Active_Buf = Usart_Buf2;
        }
        else
        {
            Active_Buf = Usart_Buf1;
        }
        Active_Count  = 0;
        // 4. 官方推荐的清除IDLE标志位方法：先读SR，再读DR
        volatile uint32_t temp;
        temp = USART1->SR;
        temp = USART1->DR;
        (void)temp; // 防止编译器警告未使用的变量
    }
}
#endif