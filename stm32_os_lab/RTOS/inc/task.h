#ifndef TASK_H
#define TASK_H

#include "projdefs.h"
#include "portmacro.h"
#include "list.h"
#include "portable.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include <string.h>

#define tskIDLE_PRIORITY    ( ( UBaseType_t ) 0U )

#define taskENTER_CRITICAL()      portENTER_CRITICAL()
#define taskEXIT_CRITICAL()       portEXIT_CRITICAL()

#define taskENTER_CRITICAL_FROM_ISR()       portSET_INTERRUPT_MASK_FROM_ISR()
#define taskEXIT_CRITICAL_FROM_ISR(x)       portCLEAR_INTERRUPT_MASK_FROM_ISR(x)

typedef struct tskTaskControlBlock
{
    volatile StackType_t *pxTopOfStack;
    ListItem_t xStateListItem;
    ListItem_t xEventListItem;
    UBaseType_t uxPriority;
    StackType_t *pxStack;
    char pcTaskName[configMAX_TASK_NAME_LEN];
}tskTCB;
typedef tskTCB TCB_t;

typedef struct xMEMORY_REGION
{
    void * pvBaseAddress;
    uint32_t ulLengthInBytes;
    uint32_t ulParameters;
} MemoryRegion_t;

typedef struct tskTaskControlBlock         * TaskHandle_t;
typedef const struct tskTaskControlBlock   * ConstTaskHandle_t;


TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,
                                const char * const pcName,
                                const configSTACK_DEPTH_TYPE uxStackDepth,
                                void * const pvParameters,
                                UBaseType_t uxPriority,
                                StackType_t * const puxStackBuffer,
                                TCB_t * const pxTaskBuffer);

BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,
                        const char * const pcName,
                        const configSTACK_DEPTH_TYPE uxStackDepth,
                        void * const pvParameters,
                        UBaseType_t uxPriority,
                        TaskHandle_t * const pxCreatedTask );

void prvInitaliseTaskLists(void);
void vTaskStartScheduler(void);
void vTaskDelete(TaskHandle_t xTaskToDelete);
BaseType_t xTaskIncrementTick(void);
void vTaskSuspend(TaskHandle_t xTaskToSuspend);
void vTaskResume(TaskHandle_t xTaskToResume);
BaseType_t vTaskResumeFromISR(TaskHandle_t xTaskToResume);
void vTaskSuspendAll(void);
BaseType_t xTaskResumeAll(void);
void vTaskDelay(const TickType_t xTicksToDelay);
void vTaskSwitchContext(void);

#endif