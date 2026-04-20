#include "task.h"

extern TaskHandle_t pxCurrentTCB;
extern TCB_t IdleTaskTCB;

List_t pxReadyTasksLists[configMAX_PRIORITIES];
List_t xDelayedTaskList1;
List_t xDelayedTaskList2;
List_t xSuspendedTaskList;
List_t xPendingReadyList;
List_t * volatile pxDelayedTaskList;
List_t * volatile pxOverflowDelayedTaskList;



static volatile UBaseType_t uxCurrentNumberOfTasks = (UBaseType_t) 0U;
static volatile BaseType_t xSchedulerRunning = pdFALSE;
static UBaseType_t uxTaskNumber = (UBaseType_t) 0U;
static volatile UBaseType_t uxTopReadyPriority = tskIDLE_PRIORITY;
static volatile TickType_t xPendedTicks = ( TickType_t ) 0U;
static volatile TickType_t xTickCount = configINITIAL_TICK_COUNT;
static volatile BaseType_t xNumOfOverflows = (BaseType_t) 0;
static volatile TickType_t xNextTaskUnblockTime = ( TickType_t ) 0U;
static volatile BaseType_t xYieldPendings[configNUMBER_OF_CORES] = {pdFALSE};

static volatile UBaseType_t uxSchedulerSuspended = (UBaseType_t) 0U;

#define taskRECORD_READY_PRIORITY(uxPriority)                           \
    do{                                                                 \
        if((uxPriority) > uxTopReadyPriority)                           \
        {                                                               \
            uxTopReadyPriority = (uxPriority);                          \
        }                                                               \
    }while(0)

#define prvAddTaskToReadyList(pxTCB)                                                            \
    do{                                                                                         \
        taskRECORD_READY_PRIORITY((pxTCB)->uxPriority);                                         \
        listINSERT_END(&(pxReadyTasksLists[(pxTCB)->uxPriority]), &((pxTCB)->xStateListItem));  \
    }while(0)

#define taskYIELD_ANY_CORE_IF_USING_PREEMPTION(pxTCB)                   \
    do{                                                                 \
        if(pxCurrentTCB->uxPriority < (pxTCB)->uxPriority)              \
        {                                                               \
            portYIELD();                                                \
        }                                                               \
        else                                                            \
        {                                                               \
            mtCOVERAGE_TEST_MARKER();                                   \
        }                                                               \
    }while(0)


#define taskSWITCH_DELAYED_LISTS()                                      \
    do{                                                                 \
        List_t * pxTemp;                                                \
        pxTemp = pxDelayedTaskList;                                     \
        pxDelayedTaskList = pxOverflowDelayedTaskList;                  \
        pxOverflowDelayedTaskList = pxTemp;                             \
        xNumOfOverflows = (BaseType_t)(xNumOfOverflows + 1);            \
        prvResetNextTaskUnblockTime();                                  \
    }while(0)

#define taskSELECT_HIGHEST_PRIORITY_TASK()                                               \
    do {                                                                                 \
        UBaseType_t uxTopPriority = uxTopReadyPriority;                                  \
        while( listLIST_IS_EMPTY( &( pxReadyTasksLists[ uxTopPriority ] ) ) != pdFALSE ) \
        {                                                                                \
            configASSERT( uxTopPriority );                                               \
            --uxTopPriority;                                                             \
        }                                                                                \
        listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB, &( pxReadyTasksLists[ uxTopPriority ] ) ); \
        uxTopReadyPriority = uxTopPriority;                                                   \
    } while( 0 ) 



static void prvInitialiseNewTask( TaskFunction_t pxTaskCode,
                                const char * const pcName,
                                const configSTACK_DEPTH_TYPE uxStackDepth,
                                void * const pvParameters,
                                UBaseType_t uxPriority,
                                TaskHandle_t * pxCreatedTask,
                                TCB_t * pxNewTCB,
                                const MemoryRegion_t * const xRegions)
{
    StackType_t *pxTopOfStack;
    UBaseType_t x;

    pxTopOfStack = pxNewTCB->pxStack + (uxStackDepth - (uint32_t)1);
    pxTopOfStack = (StackType_t *)(((uint32_t)pxTopOfStack) & (~((uint32_t) 0x0007)));
    if(pcName!=NULL)
    {
        for(x=(UBaseType_t)0; x<(UBaseType_t)configMAX_TASK_NAME_LEN; x++)
        {
            pxNewTCB->pcTaskName[x] = pcName[x];
            if(pcName[x] == (char)0x00) break;
        }
        pxNewTCB->pcTaskName[configMAX_TASK_NAME_LEN-1U] = '\0';
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    if(uxPriority >= configMAX_PRIORITIES)
    {
        uxPriority = (UBaseType_t)configMAX_PRIORITIES-(UBaseType_t)1;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }
    pxNewTCB->uxPriority = uxPriority;

    vListInitialiseItem(&(pxNewTCB->xStateListItem));
    vListInitialiseItem(&(pxNewTCB->xEventListItem));
    listSET_LIST_ITEM_OWNER(&(pxNewTCB->xStateListItem), pxNewTCB);
    listSET_LIST_ITEM_OWNER(&(pxNewTCB->xEventListItem), pxNewTCB);
    listSET_LIST_ITEM_VALUE(&(pxNewTCB->xEventListItem), (TickType_t)(configMAX_PRIORITIES-pxNewTCB->uxPriority));

    pxNewTCB->pxTopOfStack = pxPortInitialiseStack(pxTopOfStack, pxNewTCB->pxStack, pxTaskCode, pvParameters);
    if((void *)pxCreatedTask != NULL)
    {
        *pxCreatedTask = (TaskHandle_t) pxNewTCB;
    }
}

void prvInitaliseTaskLists(void)
{
    UBaseType_t uxPriority;
    for(uxPriority = (UBaseType_t)0U; uxPriority < (UBaseType_t)configMAX_PRIORITIES; uxPriority++)
    {
        vListInitalise(&(pxReadyTasksLists[uxPriority]));
    }
    vListInitalise(&xDelayedTaskList1);
    vListInitalise(&xDelayedTaskList2);
    vListInitalise(&xPendingReadyList);

    pxDelayedTaskList = &xDelayedTaskList1;
    pxOverflowDelayedTaskList = &xDelayedTaskList2;
    // 这里可能需要完善
}

static void prvAddNewTaskToReadyList(TCB_t * pxNewTCB)
{
    taskENTER_CRITICAL();
    {
        uxCurrentNumberOfTasks += (UBaseType_t)1U;
        if(pxCurrentTCB == NULL)
        {
            pxCurrentTCB = pxNewTCB;
            if(uxCurrentNumberOfTasks == (UBaseType_t) 1U)
            {
                prvInitaliseTaskLists();
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        else
        {
            if(xSchedulerRunning == pdFALSE)
            {
                if(pxCurrentTCB->uxPriority <= pxNewTCB->uxPriority)
                {
                    pxCurrentTCB = pxNewTCB;
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }

        uxTaskNumber++;

        prvAddTaskToReadyList(pxNewTCB);
    }
    taskEXIT_CRITICAL();

    if(xSchedulerRunning != pdFALSE)
    {
        taskYIELD_ANY_CORE_IF_USING_PREEMPTION(pxNewTCB);
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

}

static BaseType_t prvCreateIdleTasks(void)
{
    // xTaskCreateStatic()
    //这里目前简单的在main.c中定义
    return pdPASS;
}

static void prvAddCurrentTaskToDelayedList(TickType_t xTicksToWait, const BaseType_t xCanBlockIndefinitely)
{
    TickType_t xTimeToWake;
    const TickType_t xConstTickCount = xTickCount;
    List_t * const pxDelayedList = pxDelayedTaskList;
    List_t * const pxOverflowDelayedList = pxOverflowDelayedTaskList;

    uxListRemove(&(pxCurrentTCB->xStateListItem));

    if((xTicksToWait == portMAX_DELAY) && (xCanBlockIndefinitely != pdFALSE))
    {
        listINSERT_END(&xSuspendedTaskList, &(pxCurrentTCB->xStateListItem));
    }
    else
    {
        xTimeToWake = xConstTickCount + xTicksToWait;
        listSET_LIST_ITEM_VALUE(&(pxCurrentTCB->xStateListItem), xTimeToWake);
        if(xTimeToWake < xConstTickCount)
        {
            vListInsert(pxOverflowDelayedList, &(pxCurrentTCB->xStateListItem));
        }
        else
        {
            vListInsert(pxDelayedList, &(pxCurrentTCB->xStateListItem));
            if(xTimeToWake < xNextTaskUnblockTime)
            {
                xNextTaskUnblockTime = xTimeToWake;
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
    }

}

static void prvResetNextTaskUnblockTime(void)
{
    if(listLIST_IS_EMPTY(pxDelayedTaskList) == pdTRUE)
    {
        xNextTaskUnblockTime = portMAX_DELAY;
    }
    else
    {
        xNextTaskUnblockTime = listGET_ITEM_VALUE_OF_HEAD_ENTRY(pxDelayedTaskList);
    }
}


#if (configSUPPORT_STATIC_ALLOCATION == 1)
TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,
                                const char * const pcName,
                                const configSTACK_DEPTH_TYPE uxStackDepth,
                                void * const pvParameters,
                                UBaseType_t uxPriority,
                                StackType_t * const puxStackBuffer,
                                TCB_t * const pxTaskBuffer)
{
    TCB_t * pxNewTCB;
    TaskHandle_t xReturn;
    if((pxTaskBuffer != NULL) && (puxStackBuffer != NULL))
    {
        pxNewTCB = (TCB_t *)pxTaskBuffer;
        // (void)memset((void *)pxNewTCB, 0x00, sizeof(TCB_t));
        pxNewTCB->pxStack = puxStackBuffer;
        prvInitialiseNewTask(pxTaskCode, pcName, uxStackDepth, pvParameters, uxPriority, &xReturn, pxNewTCB, NULL);
        prvAddNewTaskToReadyList(pxNewTCB);
    }
    else
    {
        xReturn = NULL;
    }
    return xReturn;
}
#endif

void vTaskDelete(TaskHandle_t xTaskToDelete)
{
    // TCB_t* pxTCB;
    // 这里任务删除
    return ;
}

void vTaskStartScheduler(void)
{
    BaseType_t xReturn;

    xReturn = prvCreateIdleTasks();
    #if (configUSE_TIMERS == 1)
    {
        if(xReturn == pdPASS)
        {
            xReturn = xTimerCreateTimerTask();
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }
    }
    #endif
    // pxCurrentTCB = &Task1TCB;
    if(xReturn == pdPASS)
    {
        portDISABLE_INTERRUPTS();
        xNextTaskUnblockTime = portMAX_DELAY;
        xSchedulerRunning = pdTRUE;
        xTickCount = (TickType_t) configINITIAL_TICK_COUNT;
        xPortStartScheduler();
    }
    else
    {
        configASSERT(xReturn);
    }
}


BaseType_t xTaskIncrementTick(void)
{
    BaseType_t xSwitchRequired = pdFALSE;
    TCB_t * pxTCB;
    TickType_t xItemValue;

    if(uxSchedulerSuspended == (UBaseType_t) 0U)
    {
        const TickType_t xConstTickCount = xTickCount + (TickType_t) 1U;
        xTickCount = xConstTickCount;
        if(xConstTickCount == (TickType_t) 0U)
        {
            taskSWITCH_DELAYED_LISTS();
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }

        if(xConstTickCount >= xNextTaskUnblockTime)
        {
            for( ; ; )
            {
                if(listLIST_IS_EMPTY(pxDelayedTaskList) != pdFALSE)
                {
                    xNextTaskUnblockTime = portMAX_DELAY;
                    break;
                }
                else
                {
                    pxTCB = listGET_OWNER_OF_HEAD_ENTRY(pxDelayedTaskList);
                    xItemValue = listGET_LIST_ITEM_VALUE(&(pxTCB->xStateListItem));
                    if(xConstTickCount < xItemValue)
                    {
                        xNextTaskUnblockTime = xItemValue;
                        break;
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }

                    listREMOVE_ITEM(&(pxTCB->xStateListItem));

                    // if(event)这里需要完善Event
                    if(listLIST_ITEM_CONTAINER(&(pxTCB->xEventListItem)) != NULL)
                    {
                        listREMOVE_ITEM(&(pxTCB->xEventListItem));
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }

                    prvAddTaskToReadyList(pxTCB);

                    // 是否开启抢占式调度器
                    #if (configUSE_PREEMPTION == 1)
                    {
                        if(pxTCB->uxPriority > pxCurrentTCB->uxPriority)
                        {
                            xSwitchRequired = pdTRUE;
                        }
                        else
                        {
                            mtCOVERAGE_TEST_MARKER();
                        }
                    }
                    #endif
                }
            }
        }

        // 是否开启同级别任务轮转调度器
        #if ((configUSE_PREEMPTION == 1) && (configUSE_TIME_SLICING == 1))
        {
            if(listCURRENT_LIST_LENGTH(&pxReadyTasksLists[pxCurrentTCB->uxPriority]) > 1U)
            {
                xSwitchRequired = pdTRUE;
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        #endif

        #if (configUSE_PREEMPTION == 1)
        {
            if(xYieldPendings[0] != pdFALSE)
            {
                xSwitchRequired = pdTRUE;
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        #endif
        // 这里基本写好了，但是跟原版不太一样
    }
    else
    {
        xPendedTicks += 1U;
    }

    return xSwitchRequired;
}

void vTaskSuspendAll(void)
{
    uxSchedulerSuspended = uxSchedulerSuspended + (UBaseType_t) 1U;
}


BaseType_t xTaskResumeAll(void)
{
    TCB_t * pxTCB = NULL;
    BaseType_t xAlreadyYielded = pdFALSE;
    taskENTER_CRITICAL();
    {
        BaseType_t xCoreID = (BaseType_t) portGET_CORE_ID();
        configASSERT(uxSchedulerSuspended);

        uxSchedulerSuspended = (UBaseType_t)(uxSchedulerSuspended - 1U);
        if(uxSchedulerSuspended == (UBaseType_t)0U)
        {
            if(uxCurrentNumberOfTasks > (UBaseType_t)0U)
            {
                while(listLIST_IS_EMPTY(&xPendingReadyList) == pdFALSE)
                {
                    pxTCB = listGET_OWNER_OF_HEAD_ENTRY((&xPendingReadyList));
                    listREMOVE_ITEM(&(pxTCB->xEventListItem));
                    listREMOVE_ITEM(&(pxTCB->xStateListItem));
                    prvAddTaskToReadyList(pxTCB);
                    if(pxTCB->uxPriority > pxCurrentTCB->uxPriority)
                    {
                        xYieldPendings[xCoreID] = pdTRUE;
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }
                }
                if(pxTCB != NULL)
                {
                    prvResetNextTaskUnblockTime();
                }
                else mtCOVERAGE_TEST_MARKER();

                TickType_t xPendedCounts = xPendedTicks;
                if(xPendedCounts > (TickType_t) 0U)
                {
                    do{
                        if(xTaskIncrementTick() != pdFALSE)
                        {
                            xYieldPendings[xCoreID] = pdTRUE;
                        }
                        else mtCOVERAGE_TEST_MARKER();

                        xPendedCounts--;
                    }while(xPendedCounts > (TickType_t) 0U);

                    xPendedTicks = 0;
                }
                else mtCOVERAGE_TEST_MARKER();

                if(xYieldPendings[xCoreID] == pdTRUE)
                {
                    xAlreadyYielded = pdTRUE;
                    portYIELD();
                }
                else mtCOVERAGE_TEST_MARKER();
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }
    }
    taskEXIT_CRITICAL();

    return xAlreadyYielded;

}


void vTaskDelay(const TickType_t xTicksToDelay)
{
    BaseType_t xAlreadyYielded = pdFALSE;
    if(xTicksToDelay > 0U)
    {
        vTaskSuspendAll();
        {
            prvAddCurrentTaskToDelayedList(xTicksToDelay, pdFALSE);
        }
        xAlreadyYielded = xTaskResumeAll();
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    if(xAlreadyYielded == pdFALSE)
    {
        portYIELD();
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }
}



void vTaskSwitchContext(void)
{
    if(uxSchedulerSuspended != (UBaseType_t) 0U)
    {
        xYieldPendings[0] = pdTRUE;
    }
    else
    {
        xYieldPendings[0] = pdFALSE;
        // taskCHECK_FOR_STACK_OVERFLOW();

        taskSELECT_HIGHEST_PRIORITY_TASK();
    }
    // 这里需要写
}

