#ifndef PORTMACRO_H
#define PORTMACRO_H

#include "stdint.h"
#include "stddef.h"
#include "FreeRTOSConfig.h"

#define portCHAR                           char
#define portFLOAT                          float
#define portDOUBLE                         double
#define portLONG                           long
#define portSHORT                          short
#define portSTACK_TYPE                     uint32_t
#define portBASE_TYPE                      long

typedef portSTACK_TYPE                     StackType_t;
typedef long                               BaseType_t;
typedef unsigned long                      UBaseType_t;

#if ( configUSE_16_BIT_TICKS == 1 )
typedef uint16_t TickType_t;
#define portMAX_DELAY ( TickType_t ) 0xffff
#else
typedef uint32_t TickType_t;
#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif


#define portDISABLE_INTERRUPTS()                vPortRaiseBASEPRI()
#define portENABLE_INTERRUPTS()                 vPortSetBASEPRI( 0 )
#define portSET_INTERRUPT_MASK_FROM_ISR()       ulPortRaiseBASEPRI()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)    vPortSetBASEPRI( x )

extern void vPortEnterCritical(void);
extern void vPortExitCritical(void);
#define portENTER_CRITICAL()                    vPortEnterCritical()
#define portEXIT_CRITICAL()                     vPortExitCritical()

#define configASSERT( x )         \
    if( ( x ) == 0 )              \
    {                             \
        portDISABLE_INTERRUPTS(); \
        for( ; ; )                \
        ;                         \
    }

#ifndef portFORCE_INLINE
    #define portFORCE_INLINE    inline __attribute__((always_inline))
#endif


#define portNVIC_INT_CTRL_REG       ( *( ( volatile uint32_t * ) 0xe000ed04 ) )
#define portNVIC_PENDSVSET_BIT      (1UL << 28UL)

#define portYIELD()                                                     \
    {                                                                   \
        portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;                 \
        __asm volatile("dsb" ::: "memory");                             \
        __asm volatile("isb");                                          \     
    }


portFORCE_INLINE static void vPortRaiseBASEPRI(void)
{
    uint32_t ulNewBASEPRI;
    __asm volatile(
        "mov %0, %1         \n"
        "msr basepri, %0    \n"
        "isb                \n"
        "dsb                \n"
        : "=r" (ulNewBASEPRI) 
        : "i" (configMAX_SYSCALL_INTERRUPT_PRIORITY) 
        : "memory"
    );
}

portFORCE_INLINE static uint32_t ulPortRaiseBASEPRI()
{
    uint32_t ulOriginalBASEPRI, ulNewBASEPRI;

    __asm volatile(
        "mrs %0, basepri    \n"
        "mov %1, %2         \n"
        "msr basepri, %1    \n"
        : "=r" (ulOriginalBASEPRI), "=r" (ulNewBASEPRI)
        : "i" (configMAX_SYSCALL_INTERRUPT_PRIORITY)
        : "memory"
    );

    return ulOriginalBASEPRI;
}


portFORCE_INLINE static void vPortSetBASEPRI(uint32_t ulNewMaskValue)
{
    __asm volatile(
        "msr basepri, %0"
        :: "r" (ulNewMaskValue) : "memory"
    );
}

#endif