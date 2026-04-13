#include "led.h"

/**
 * @brief Initialize 2 led which pins are GPIOA_8 and GPIOD_2
 * @param None
 * @retval None
 */
void LedInit(void)
{
    RCC_APB2PeriphClockCmd(LED1_GPIO_CLK | LED2_GPIO_CLK, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructrue;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructrue.GPIO_Pin = LED1_GPIO_PIN;
    GPIO_InitStructrue.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructrue);
    GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);
    GPIO_InitStructrue.GPIO_Pin = LED2_GPIO_PIN;
    GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructrue);
    GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);
}


