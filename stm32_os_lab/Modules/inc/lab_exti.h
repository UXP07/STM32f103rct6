#ifndef _LAB_EXTI_H_
#define _LAB_EXTI_H_

#include "stm32f10x_exti.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include "led.h"

#define KEY1_INT_GPIO_PORT      GPIOC
#define KEY1_INT_GPIO_PIN       GPIO_Pin_5
#define KEY1_INT_GPIO_CLK       (RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO)

#define KEY2_INT_GPIO_PORT      GPIOA
#define KEY2_INT_GPIO_PIN       GPIO_Pin_15
#define KEY2_INT_GPIO_CLK       (RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO)

#define LAB_EXTI_CODE           0


void NVIC_EXTI_Configuration(void);
void EXTI_Configuration(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);
#if LAB_EXTI_CODE
void lab_exti(void);
#endif

#endif