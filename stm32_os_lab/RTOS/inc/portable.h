#ifndef PORT_H
#define PORT_H

#include "portmacro.h"
#include "projdefs.h"
#include "task.h"

#define pvPortMallocStack   pvPortMalloc
#define vPortFreeStack      vPortFreeStack

#ifdef portBYTE_ALIGNMENT == 8
    #define portBYTE_ALIGNMENT_MASK     0x007
#endif

void vPortSetupTimerInterrupt(void);
StackType_t* pxPortInitialiseStack( StackType_t* pxTopOfStack, 
                                    StackType_t* pxEndOfStack,
                                    TaskFunction_t pxCode,
                                    void * pvParameters);

BaseType_t xPortStartScheduler(void);
void xPortSysTickHandler(void);
void vPortSVCHandler(void);
void xPortPendSVHandler(void);
void * pvPortMalloc(size_t xWantedSize);
void vPortFree(void * pv);
void * pvPortMalloc(size_t xWantedSize);
void vPortFree(void * pv);

#endif