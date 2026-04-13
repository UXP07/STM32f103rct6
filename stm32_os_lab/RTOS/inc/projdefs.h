#ifndef PROJDEF_H
#define PROJDEF_H

typedef void (* TaskFunction_t)(void * arg);

#define pdFALSE                             ((BaseType_t) 0 )
#define pdTRUE                              ((BaseType_t) 1 )

#define pdPASS                              ( pdTRUE )
#define pdFAIL                              ( pdFALSE )

#endif