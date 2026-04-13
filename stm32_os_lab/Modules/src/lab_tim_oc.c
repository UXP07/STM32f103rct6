#include "lab_tim_oc.h"

void Pwm_Led_Init(void)
{
    RCC_APB2PeriphClockCmd(GENERAL_TIM_CLK|PWM_LED_CLK, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructrue;
    GPIO_InitStructrue.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructrue.GPIO_Pin = PWM_LED_PIN;
    GPIO_InitStructrue.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PWM_LED_PORT, &GPIO_InitStructrue);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72-1;
    TIM_TimeBaseInitStructure.TIM_Period = 100-1;
    // TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseInitStructure);
    
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OC1Init(GENERAL_TIM, &TIM_OCInitStructure);

    // TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1预装载使能	 
	// TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIMx在ARR上的预装载寄存器
    
    TIM_Cmd(GENERAL_TIM, ENABLE);
    TIM_CtrlPWMOutputs(GENERAL_TIM, ENABLE);    // 高级定时器使能
}

#if LAB_TIM_OC

void lab_tim_oc(void)
{
    while(1)
    {
        for(int i=0; i<100; i++)
        {
            TIM_SetCompare1(GENERAL_TIM, i);
            delay_ms(10);
        }
        for(int i=99; i>=0; i--)
        {
            TIM_SetCompare1(GENERAL_TIM, i);
            delay_ms(10);
        }
    }
}

#endif