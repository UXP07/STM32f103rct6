#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "task.h"

#define queueQUEUE_TYPE_BASE            ((uint8_t) 0U)

struct QueueDefinition;
typedef struct QueueDefinition * QueueHandle_t;
typedef struct QueueDefinition * QueueSetHandle_t;
typedef struct QueueDefinition * QueueSetMemberHandle_t;

#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
    #define xQueueCreate(uxQueueLength, uxItemSize)         xQueueGenericCreate((uxQueueLength), (uxItemSize), (queueQUEUE_TYPE_BASE))
#endif

#if (configSUPPORT_STATIC_ALLOCATION == 1)
    #define xQueueCreateStatic( uxQueueLength, uxItemSize, pucQueueStorage, pxQueueBuffer )    xQueueGenericCreateStatic( ( uxQueueLength ), ( uxItemSize ), ( pucQueueStorage ), ( pxQueueBuffer ), ( queueQUEUE_TYPE_BASE ) )
#endif

#endif