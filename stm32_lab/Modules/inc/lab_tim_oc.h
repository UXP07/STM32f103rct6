#ifndef LAB_TIM_OC_H
#define LAB_TIM_OC_H

#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include "stm32f10x.h"
#include "delay.h"


#define GENERAL_TIM         TIM1
#define GENERAL_TIM_CLK     RCC_APB2Periph_TIM1

#define PWM_LED_PORT        GPIOA
#define PWM_LED_PIN         GPIO_Pin_8
#define PWM_LED_CLK         RCC_APB2Periph_GPIOA

#define LAB_TIM_OC          0

void Pwm_Led_Init(void);
void lab_tim_oc(void);

#endif