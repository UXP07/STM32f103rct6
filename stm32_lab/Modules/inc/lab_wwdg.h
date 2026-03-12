#ifndef LAB_WWDG_H
#define LAB_WWDG_H

#include "stm32f10x_rcc.h"
#include "stm32f10x_wwdg.h"
#include "misc.h"
#include "led.h"

#define WWDG_PRE            WWDG_Prescaler_8
#define WWDG_WR             0x5F
#define WWDG_TR             0x7F
#define WWDG_CNT            0x7F
#define LAB_WWDG            0

void WWDG_Init(uint32_t prv, uint8_t tr, uint8_t wr);
void WWDG_IRQHandler(void);

#endif