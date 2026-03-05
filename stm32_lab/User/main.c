#include "main.h"

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
