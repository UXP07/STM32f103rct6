#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define configSUPPORT_STATIC_ALLOCATION         1

#define configCPU_CLOCK_HZ    ( ( unsigned long ) 72000000 )

#define configUSE_16_BIT_TICKS                  0
#define configNUMBER_OF_CORES                   1
#define configMAX_TASK_NAME_LEN                 16
#define configSTACK_DEPTH_TYPE                  uint32_t
#define configMAX_PRIORITIES                    32
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    (5<<4)
#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  1
#define configUSE_TIMERS                        0
#define configINITIAL_TICK_COUNT                0
#define configTOTAL_HEAP_SIZE                   4096U
#define configAPPLICATION_ALLOCATED_HEAP        0
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configSUPPORT_STATIC_ALLOCATION         1

#define xPortPendSVHandler                      PendSV_Handler
#define xPortSysTickHandler                     SysTick_Handler
#define vPortSVCHandler                         SVC_Handler

#define INCLUDE_vTaskDelete                     0

#endif/* FREERTOS_CONFIG_H */