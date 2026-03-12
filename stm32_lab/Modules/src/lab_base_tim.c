#include "lab_base_tim.h"

void Basic_Tim_Init(void)
{
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = BASIC_TIM_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    RCC_APB1PeriphClockCmd(BASIC_TIM_CLK, ENABLE);
    TIM_TimeBaseInitStructure.TIM_Prescaler = 7200-1;
    TIM_TimeBaseInitStructure.TIM_Period = 10000;
    TIM_TimeBaseInit(BASIC_TIM, &TIM_TimeBaseInitStructure);
    TIM_ClearFlag(BASIC_TIM, TIM_FLAG_Update);
    TIM_ITConfig(BASIC_TIM, TIM_IT_Update, ENABLE);
    TIM_Cmd(BASIC_TIM, ENABLE);

}

void TIM6_IRQHandler(void)
{
    LED1_TOGGLE;
    LED2_TOGGLE;
    TIM_ClearITPendingBit(BASIC_TIM, TIM_IT_Update);
}