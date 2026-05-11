#ifndef _LAB_USART_SIMPLE__H_
#define _LAB_USART_SIMPLE__H_

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include "delay.h"
#include "led.h"
#include <stdio.h>
#include "lab_usart_ringbuffer.h"

#define DEBUG_USART             USART1
#define DUBUG_USART_CLK         RCC_APB2Periph_USART1
#define DEBUG_USART_BD          9600

#define DEBUG_USART_GPIO_CLK    RCC_APB2Periph_GPIOA
#define DEBUG_USART_TX_PORT     GPIOA
#define DEBUG_USART_TX_PIN      GPIO_Pin_9
#define DEBUG_USART_RX_PORT     GPIOA
#define DEBUG_USART_RX_PIN      GPIO_Pin_10

#define DEBUG_USART_IRQ         USART1_IRQn
#define DEBUG_USART_IRQHandler  USART1_IRQHandler

#define LAB_USART_CODE          0

void NVIC_USART_Configuration(void);
void Uart_Init(void);
void Usart_SendByte(USART_TypeDef* USARTx, uint16_t Data);
void Usart_SendString(USART_TypeDef* USARTx, char* str);
void DEBUG_USART_IRQHandler(void);
#if LAB_USART_CODE
void lab_usart_send(void);
#endif

#endif