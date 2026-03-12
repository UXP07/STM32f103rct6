#ifndef LAB_BASE_TIM_H
#define LAB_BASE_TIM_H

#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "stm32f10x.h"
#include "led.h"

#define            BASIC_TIM                   TIM6
#define            BASIC_TIM_CLK               RCC_APB1Periph_TIM6
#define            BASIC_TIM_IRQ               TIM6_IRQn
#define            BASIC_TIM_IRQHandler        TIM6_IRQHandler

void Basic_Tim_Init(void);
void TIM6_IRQHandler(void);

#endif