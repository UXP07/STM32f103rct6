#ifndef __DELAY_H
#define __DELAY_H 			   

#include "sys.h"
#include "misc.h"

#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"
#include "task.h"
extern void xPortSysTickHandler(void);
#endif

void SysTick_Handler(void);
void delay_init(void);
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);



#endif





















































