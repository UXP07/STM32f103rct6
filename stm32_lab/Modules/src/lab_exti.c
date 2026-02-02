#include "lab_exti.h"

/**
 * @brief: Configuration of NVIC
 */
void NVIC_EXTI_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief: Configuration of EXTI, which connect GPIO(key) & AFIO & EXTI & NVIC
 */
void EXTI_Configuration(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(KEY1_INT_GPIO_CLK|KEY2_INT_GPIO_CLK, ENABLE);

    // NVIC_Configuration();

    // Config key1
    GPIO_InitStructure.GPIO_Pin = KEY1_INT_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5);
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    // Config key2
    GPIO_InitStructure.GPIO_Pin = KEY2_INT_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(KEY2_INT_GPIO_PORT, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource15);
    EXTI_InitStructure.EXTI_Line = EXTI_Line15;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

}

void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetFlagStatus(EXTI_Line5) != RESET){
        LED1_TOGGLE;
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
}

void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetFlagStatus(EXTI_Line15) != RESET){
        LED1_TOGGLE;
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}


/**
 * @brief: This lab show that led will toggle when we press the key. 
 *         Of course you can config different situations in the handle function
 */
void lab_exti(void)
{
    LedInit();
    EXTI_Configuration();
    NVIC_EXTI_Configuration();
}