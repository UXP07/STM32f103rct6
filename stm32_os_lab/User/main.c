#include "main.h"

extern List_t pxReadyTasksLists[configMAX_PRIORITIES];

void delay_circu(uint32_t count);
void Task1_Entry(void *p_arg);
void Task2_Entry(void *p_arg);


uint8_t flag1 = 0;
uint8_t flag2 = 0;

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



    // for(uint8_t i=0; i<100; i++)
    // {
    //     printf("Hello world %d\n", i);
    //     delay_ms(500);
    // }

    TaskHandle_t task1 = xTaskCreateStatic(Task1_Entry, "Task1", TASK1_STACK_SIZE, (void*)NULL, 0, Task1Stack, (TCB_t*)&Task1TCB);
    // vListInsertEnd(&(pxReadyTasksLists[0]), &(task1->xStateListItem));
    TaskHandle_t task2 = xTaskCreateStatic(Task2_Entry, "Task2", TASK2_STACK_SIZE, (void*)NULL, 0, Task2Stack, (TCB_t*)&Task2TCB);
    // vListInsertEnd(&(pxReadyTasksLists[0]), &(task2->xStateListItem));
    
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
        flag1 = 1;
        delay_circu(100);
        flag1 = 0;
        delay_circu(100);
    }
}

void Task2_Entry(void *p_arg)
{
    while(1)
    {
        flag2 = 1;
        delay_circu(100);
        flag2 = 0;
        delay_circu(100);
    }
}
