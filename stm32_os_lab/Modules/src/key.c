#include"key.h"

/**
 * @brief Initialize key0 and key1 which pins are GPIOC_5 and GPIOA_15
 * @param None
 * @retval None
 */
void KeyInit(void)
{
    RCC_APB2PeriphClockCmd(KEY0_GPIO_CLK|KEY1_GPIO_CLK, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = KEY0_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(KEY0_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;
    GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * @brief Check the key's voltage level
 * @param GPIOx: KEY0_GPIO_PORT, KEY1_GPIO_PORT
 * @param GPIO_Pin: KEY0_GPIO_PIN, KEY1_GPIO_PIN
 * @retval Low level(0) or High level(1)
 */
uint8_t keyScan(GPIO_TypeDef* GPIOx,  uint16_t GPIO_Pin)
{
    return GPIO_ReadInputDataBit(GPIOx, GPIO_Pin);
}