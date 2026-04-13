#include "lab_dma.h"

// extern int count;

void Dma_m2m_Init(DMA_Channel_TypeDef* DMA_CHx, uint32_t* source, uint32_t* target)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(DMA_INT_CLK, ENABLE);
    DMA_DeInit(DMA_CHANNEL);

    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)source;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)target;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    
    DMA_Init(DMA_CHx, &DMA_InitStructure);
}

void NVIC_DMA_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void Dma_m2p_Init(DMA_Channel_TypeDef* DMA_CHx, uint32_t* source)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(DMA_INT_CLK, ENABLE);
    DMA_DeInit(DMA_CHANNEL);

    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)source;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    
    DMA_Init(DMA_CHx, &DMA_InitStructure);
    DMA_ITConfig(DMA_CHANNEL, DMA_IT_TC, ENABLE);
    NVIC_DMA_Configuration();
    USART_DMACmd(DEBUG_USART, USART_DMAReq_Tx, ENABLE);
}


void Dma_Rework(void)
{
    DMA_Cmd(DMA_CHANNEL, DISABLE);
    DMA_SetCurrDataCounter(DMA_CHANNEL, BUFFER_SIZE);
    DMA_Cmd(DMA_CHANNEL, ENABLE);
}



#if LAB_DMA_CODE
void lab_dma_simple(void)
{
    delay_init();
    delay_ms(300);
    Uart_Init();
    LedInit();

    uint32_t aSRC_Buffer[BUFFER_SIZE]= {
        0x01,0x01,0x01,0x01,
        0x01,0x01,0x01,0x01,
        0x01,0x01,0x01,0x01,
        0x01,0x01,0x01,0x01,
        0x01,0x01,0x01,0x01,
        0x01,0x01,0x01,0x01,
        0x01,0x01,0x01,0x01,
        0x01,0x01,0x01,0x01
    };
    uint32_t aTAR_Buffer[BUFFER_SIZE]= {
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00
    };

    Dma_m2m_Init(DMA_CHANNEL, aSRC_Buffer, aTAR_Buffer);

    Dma_Rework();

    for(int i=0; i<20; i++)
    {
        while(DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);
        DMA_ClearFlag(DMA1_FLAG_TC1);
        for(int j=0; j<32; j++)
        {
            printf("%ld ", aTAR_Buffer[j]);
        }
        printf("\n");
        for(int j=0; j<32; j++)
        {
            aSRC_Buffer[j]++;
        }
        Dma_Rework();
        delay_ms(1000);
    }
    DMA_Cmd(DMA_CHANNEL, DISABLE);
}



// DMA+USART
// -----------------------------------------------------


uint8_t aSRC_Buffer[BUFFER_SIZE]= "Hello world\n";
int count = 10;

int main(void)
{

    delay_init();
    delay_ms(300);
    Uart_Init();
    LedInit();
    Dma_m2p_Init(DMA_CHANNEL, (uint32_t*)aSRC_Buffer);
    Dma_Rework();

    while(1)
    {
        // PAout(8) = 0;
        // delay_ms(500);
        // PAout(8) = 1;
        // delay_ms(500);
    }

    return 0;
}

extern int count;
void DMA1_Channel4_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC4))
    {
        DMA_ClearITPendingBit(DMA1_IT_TC4);
        if(count)
        {
            // for(int i=0; i<32; i++)
            // {
            //     aSRC_Buffer[i]++;
            // }
            count--;
            Dma_Rework();
        }
        else
        {
            DMA_Cmd(DMA_CHANNEL, DISABLE);
        }
        
    }
    else
    {
        printf("error\n");
    }
}
#endif