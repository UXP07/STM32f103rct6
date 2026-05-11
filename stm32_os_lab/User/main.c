#include "main.h"

extern List_t pxReadyTasksLists[configMAX_PRIORITIES];

void delay_circu(uint32_t count);
void Task1_Entry(void *p_arg);
void Task2_Entry(void *p_arg);
void prvIdleTask(void *p_arg);



#define configMINIMAL_STACK_SIZE    128
#define TASK1_STACK_SIZE        128
#define TASK2_STACK_SIZE        128

StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
StackType_t Task1Stack[TASK1_STACK_SIZE];
StackType_t Task2Stack[TASK2_STACK_SIZE];

TCB_t IdleTaskTCB;
TCB_t Task1TCB;
TCB_t Task2TCB;

TaskHandle_t pxCurrentTCB = NULL;


int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    Uart_Init();
    LedInit();
    
    TaskHandle_t xTask1Handle = xTaskCreateStatic(Task1_Entry, "Task1", TASK1_STACK_SIZE, (void*)NULL, 3, Task1Stack, (TCB_t*)&Task1TCB);
    // vListInsertEnd(&(pxReadyTasksLists[0]), &(task1->xStateListItem));
    TaskHandle_t xTask2Handle = xTaskCreateStatic(Task2_Entry, "Task2", TASK2_STACK_SIZE, (void*)NULL, 2, Task2Stack, (TCB_t*)&Task2TCB);
    // vListInsertEnd(&(pxReadyTasksLists[0]), &(task2->xStateListItem));
    TaskHandle_t xIdleTaskHandle = xTaskCreateStatic(prvIdleTask, "Idle", configMINIMAL_STACK_SIZE, (void*)NULL, 0, IdleTaskStack, (TCB_t*)&IdleTaskTCB);
    
    pxCurrentTCB = xTask1Handle;
    // printf("Hello\n");
    vTaskStartScheduler();

    // for(uint8_t i=0; i<100; i++)
    // {
    //     printf("Hello world %d\n", i);
    //     delay_ms(500);
    // }

    while(1)
    {
        
    }


    return 0;
}

void delay_circu(uint32_t count)
{
    for(uint32_t i=0; i<count; i++);
}

void Task1_Entry(void *p_arg)
{
    while(1)
    {
        LED2_TOGGLE;
        // printf("Hello I'm 1\n");
        vTaskDelay(500);
    }
}

void Task2_Entry(void *p_arg)
{
    while(1)
    {
        LED1_TOGGLE;
        // printf("Hello I'm 2\n");
        vTaskDelay(500);
    }
}

void prvIdleTask(void *p_arg)
{
    while(1)
    {

    }
}
