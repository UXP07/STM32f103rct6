#ifndef _LAB_DMA_H_
#define _LAB_DMA_H_

#include "stm32f10x.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_rcc.h"
#include "lab_usart_simple.h"

#define DMA_INT_CLK         RCC_AHBPeriph_DMA1
#define DMA_CHANNEL         DMA1_Channel4
#define BUFFER_SIZE         12
#define LAB_DMA_CODE        0



void Dma_m2m_Init(DMA_Channel_TypeDef* DMA_CHx, uint32_t* source, uint32_t* target);
void Dma_Rework(void);
void NVIC_DMA_Configuration(void);
void Dma_m2p_Init(DMA_Channel_TypeDef* DMA_CHx, uint32_t* source);


#if LAB_DMA_SIMPLE_CODE
void lab_dma_simple(void);
#endif

#endif