#ifndef LED_H
#define LED_H

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

// 定义led引脚 
#define LED1_GPIO_CLK   RCC_APB2Periph_GPIOA
#define LED1_GPIO_PORT  GPIOA
#define LED1_GPIO_PIN   GPIO_Pin_8

#define LED2_GPIO_CLK   RCC_APB2Periph_GPIOD
#define LED2_GPIO_PORT  GPIOD
#define LED2_GPIO_PIN   GPIO_Pin_2

/**
 * 1 - off
 * 0 - on
 */
#define ON  0
#define OFF 1

// 使用固件库控制led 
#define LED1(status) if(status) \
        GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN); \
        else \
        GPIO_ResetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);

#define LED2(status) if(status) \
        GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN); \
        else \
        GPIO_ResetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);

// 寄存器操作IO
#define digitalHi(p, i)     {p->ODR |= i;}  // 输出为高电平
#define digitalLo(p, i)     {p->ODR &= ~i;}   // 输出为低电平
// #define digitalHi(p, i)     {p->BSRR = i;}  // 输出为高电平
// #define digitalLo(p, i)     {p->BRR = i;}   // 输出为低电平
#define digitalToggle(p, i) {p->ODR ^= i;} // 反转电平

// 宏定义控制IO
#define LED1_ON         digitalLo(LED1_GPIO_PORT, LED1_GPIO_PIN)
#define LED1_OFF        digitalHi(LED1_GPIO_PORT, LED1_GPIO_PIN)
#define LED1_TOGGLE     digitalToggle(LED1_GPIO_PORT, LED1_GPIO_PIN)

#define LED2_ON         digitalLo(LED2_GPIO_PORT, LED2_GPIO_PIN)
#define LED2_OFF        digitalHi(LED2_GPIO_PORT, LED2_GPIO_PIN)
#define LED2_TOGGLE     digitalToggle(LED2_GPIO_PORT, LED2_GPIO_PIN)


void LedInit(void);

#endif