#include "main.h"


int main(void)
{

    delay_init();
    delay_ms(300);
    Uart_Init();
    LedInit();
    // for(int i=0; i<100; i++)
    // { 
    //     Usart_SendString(USART1, "Hello I'm 32\n");
    //     delay_ms(1000);
    // }




    

    while(1)
    {
        // PAout(8) = 0;
        // delay_ms(500);
        // PAout(8) = 1;
        // delay_ms(500);
    }

    return 0;
}
