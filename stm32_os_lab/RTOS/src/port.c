#include "portable.h"

static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;

#define portNVIC_SYSTICK_CTRL_REG             ( *( ( volatile uint32_t * ) 0xe000e010 ) )
#define portNVIC_SYSTICK_LOAD_REG             ( *( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG    ( *( ( volatile uint32_t * ) 0xe000e018 ) )
#define portNVIC_SHPR3_REG                    (*(( volatile uint32_t *) 0xe000ed20))
#define portNVIC_SHPR2_REG                    ( *( ( volatile uint32_t * ) 0xe000ed1c ) )
#define portMIN_INTERRUPT_PRIORITY              ( 255UL )
#define portNVIC_PENDSV_PRI                     (((uint32_t) portMIN_INTERRUPT_PRIORITY ) << 16UL)
#define portNVIC_SYSTICK_PRI                    (((uint32_t) portMIN_INTERRUPT_PRIORITY ) << 24UL )

#define configSYSTICK_CLOCK_HZ                  (configCPU_CLOCK_HZ)
#define configTICK_RATE_HZ                      1000

#define portNVIC_SYSTICK_CLK_BIT_CONFIG         (1UL << 2UL)
#define portNVIC_SYSTICK_INT_BIT                (1UL << 1UL)
#define portNVIC_SYSTICK_ENABLE_BIT             (1UL << 0UL)

#define portINITIAL_XPSR        (0x01000000)
#define portSTART_ADDRESS_MASK  ((StackType_t) 0xfffffffeUL)

#define portVECTACTIVE_MASK                   ( 0xFFUL )


static void prvTaskExitError(void)
{
    portDISABLE_INTERRUPTS();
    for(;;);
}

StackType_t* pxPortInitialiseStack( StackType_t* pxTopOfStack, 
                                    StackType_t* pxEndOfStack,
                                    TaskFunction_t pxCode,
                                    void * pvParameters)
{
    pxTopOfStack--;
    *pxTopOfStack = portINITIAL_XPSR;
    pxTopOfStack--;
    *pxTopOfStack = ((StackType_t)pxCode) & portSTART_ADDRESS_MASK;
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t)prvTaskExitError;
    pxTopOfStack -= 5;
    *pxTopOfStack = (StackType_t)pvParameters;
    pxTopOfStack -= 8;
    return pxTopOfStack;
}

__attribute__((weak)) void vPortSetupTimerInterrupt(void)
{
    portNVIC_SYSTICK_CTRL_REG = 0UL;
    portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
    portNVIC_SYSTICK_LOAD_REG = (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ) - 1UL;
    portNVIC_SYSTICK_CTRL_REG = ( portNVIC_SYSTICK_CLK_BIT_CONFIG | portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT );
}

/**
 * @brief: xPortSysTickHandler == SysTick_Handler, use it in systick Interrupt
 */
void xPortSysTickHandler(void)
{
    portDISABLE_INTERRUPTS();
    // traceISR_ENTER();
    {
        if(xTaskIncrementTick() != pdFALSE)
        {
            portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
        }
        else
        {
            // traceISR_EXIT();
            mtCOVERAGE_TEST_MARKER();
        }
    }

    portENABLE_INTERRUPTS();

}




/**
 * @brief: Seek MSP, Enable Interrupt, Trigger SVC
 * @brief: 在 CM3 中，SVC 异常是不受 basepri 屏蔽的，但受 PRIMASK 屏蔽，所以必须先 cpsie i 才能进 SVC 中断。
 *         此时如果 basepri 被设置过，PendSV 和 SysTick 依然是挂起状态。
 */
static void prvPortStartFirstTask(void)
{
    __asm volatile(
        "ldr r0, =0xE000ED08    \n"
        "ldr r0, [r0]           \n"
        "ldr r0, [r0]           \n"
        "msr msp, r0            \n"
        "cpsie i                \n"
        "cpsie f                \n"
        "dsb                    \n"
        "isb                    \n"
        "svc 0                  \n"
        "nop                    \n"
        ".ltorg                 \n"
    );
}


/**
 * @brief: Switch MSP to PSP, then start first task
 */
void vPortSVCHandler(void)
{
    __asm volatile(
        "ldr r3, pxCurrentTCBConst2     \n"
        "ldr r1, [r3]                   \n"
        "ldr r0, [r1]                   \n"
        "ldmia r0!, {r4-r11}            \n"
        "msr psp, r0                    \n"
        "isb                            \n"
        "mov r0, #0                     \n"
        "msr basepri, r0                \n"
        "orr r14, #0xd                  \n"
        "bx r14                         \n"
        "                               \n"
        ".align 4                       \n"
        "pxCurrentTCBConst2: .word pxCurrentTCB     \n"
    );
}


/**
 * @brief: Set Priority and start first task
 */
BaseType_t xPortStartScheduler(void)
{
    // 这里可能需要完善
    portNVIC_SHPR3_REG |= portNVIC_PENDSV_PRI;
    portNVIC_SHPR3_REG |= portNVIC_SYSTICK_PRI;
    portNVIC_SHPR2_REG = 0;

    vPortSetupTimerInterrupt();

    uxCriticalNesting = 0;

    prvPortStartFirstTask();
    prvTaskExitError();

    return 0;
}



void xPortPendSVHandler(void)
{
    __asm volatile(
        "mrs r0, psp                    \n"
        "isb                            \n"
        "                               \n"
        "ldr r3, pxCurrentTCBConst      \n"
        "ldr r2, [r3]                   \n"
        "                               \n"
        "stmdb r0!, {r4-r11}            \n"
        "str r0, [r2]                   \n"
        "                               \n"
        "stmdb sp!, {r3, r14}           \n"
        "mov r0, %0                     \n"
        "msr basepri, r0                \n"
        "bl vTaskSwitchContext          \n"
        "mov r0, #0                     \n"
        "msr basepri, r0                \n"
        "ldmia sp!, {r3, r14}           \n"
        "                               \n"
        "ldr r1, [r3]                   \n"
        "ldr r0, [r1]                   \n"
        "ldmia r0!, {r4-r11}            \n"
        "msr psp, r0                    \n"
        "isb                            \n"
        "bx r14                         \n"
        "                               \n"
        ".align 4                       \n"
        "pxCurrentTCBConst: .word pxCurrentTCB \n"
        ::"i" (configMAX_SYSCALL_INTERRUPT_PRIORITY)
    );
}



/**
 * @brief: Enter the Critical
 * @brief: define in portmacro.h
 */
void vPortEnterCritical(void)
{
    portDISABLE_INTERRUPTS();
    uxCriticalNesting++;
    if(uxCriticalNesting == 1)
    {
        configASSERT((portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK) == 0);
    }

}

/**
 * @brief: Exit the Critical
 * @brief: define in portmacro.h
 */
void vPortExitCritical(void)
{
    configASSERT(uxCriticalNesting);
    uxCriticalNesting--;
    if(uxCriticalNesting == 0)
    {
        portENABLE_INTERRUPTS();
    }
}

