#ifndef PORT_H
#define PORT_H

#include "portmacro.h"
#include "projdefs.h"
#include "task.h"


void vPortSetupTimerInterrupt(void);
StackType_t* pxPortInitialiseStack( StackType_t* pxTopOfStack, 
                                    StackType_t* pxEndOfStack,
                                    TaskFunction_t pxCode,
                                    void * pvParameters);

BaseType_t xPortStartScheduler(void);
void xPortSysTickHandler(void);
void vPortSVCHandler(void);
void xPortPendSVHandler(void);

#endif