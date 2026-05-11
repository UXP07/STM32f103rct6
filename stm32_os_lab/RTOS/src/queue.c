#include "queue.h"

#define queueUNLOCKED                           ((int8_t) -1)
#define queueLOCKED_UNMODIFIED                  ((int8_t) 0)
#define queueINT8_MAX                           ((int8_t) 127)

#define uxQueueType                             pxHead
#define queueQUEUE_IS_MUTEX                     NULL

typedef struct QueuePointers
{
    int8_t * pcTail;
    int8_t * pcReadFrom;
} QueuePointers_t;

typedef struct SemaphoreData
{
    TaskHandle_t xMutexHolder;
    UBaseType_t uxRecursiveCallCount;
} SemaphoreData_t;

#define queueYIELD_IF_USING_PREEMPTION() portYIELD()

typedef struct QueueDefinition
{
    int8_t * pcHead;
    int8_t * pcWriteTo;

    union
    {
        QueuePointers_t xQueue;
        SemaphoreData_t xSemaphore;
    } u;

    List_t xTasksWaitingToSend;
    List_t xTasksWaitingToReceive;

    volatile UBaseType_t uxMessagesWaiting;
    UBaseType_t uxLength;
    UBaseType_t uxItemSize;

    volatile int8_t cRxLock;
    volatile int8_t cTxLock;

    uint8_t ucStaticallyAllocated;

    struct QueueDefinition * pxQueueSetContainer;

} xQUEUE;

typedef xQUEUE Queue_t;




