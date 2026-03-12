#ifndef LAB_IWDG_H
#define LAB_IWDG_H

#include "stm32f10x_iwdg.h"
#include "stm32f10x_rcc.h"

#define IWDG_PRE        IWDG_Prescaler_64
#define IWDG_RELOAD     625
#define LAB_IWDG        0

void IWDG_Init(uint8_t prv, uint16_t rlv);
void IWDG_Feed(void);

#endif