#include "lab_wwdg.h"

void WWDG_Init(uint32_t prv, uint8_t tr, uint8_t wr)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

    NVIC_InitTypeDef   NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    WWDG_SetCounter(tr);
    WWDG_SetWindowValue(wr);
    WWDG_SetPrescaler(prv);
    WWDG_ClearFlag();
    WWDG_EnableIT();
    WWDG_Enable(WWDG_CNT);
}

void WWDG_IRQHandler(void)
{
    WWDG_ClearFlag();
    LED2_ON;
}


#if LAB_WWDG

WWDG_Init(WWDG_PRE, WWDG_TR, WWDG_WR);
#endif