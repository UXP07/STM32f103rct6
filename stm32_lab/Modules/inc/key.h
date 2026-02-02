#ifndef __KEY_H
#define __KEY_H

#include"stm32f10x.h"
#include"stm32f10x_rcc.h"
#include"stm32f10x_gpio.h"

#define KEY0_GPIO_CLK   RCC_APB2Periph_GPIOC
#define KEY0_GPIO_PORT  GPIOC
#define KEY0_GPIO_PIN    GPIO_Pin_5

#define KEY1_GPIO_CLK   RCC_APB2Periph_GPIOA
#define KEY1_GPIO_PORT  GPIOA
#define KEY1_GPIO_PIN    GPIO_Pin_15

/**
 * Get 0 when push key down
 * Get 1 when nothing to do
 */
#define KEY_ON  0
#define KEY_OFF 1

void KeyInit(void);
uint8_t keyScan(GPIO_TypeDef* GPIOx,  uint16_t GPIO_Pin);

#endif