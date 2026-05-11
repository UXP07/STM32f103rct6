#ifndef _LAB_USART_RB_H_
#define _LAB_USART_RB_H_

#include <stdint.h>
#include <stdbool.h>
#include "led.h"

#define USE_RINGBUFFER              0
#define UART_RX_BUF_SIZE            20

typedef struct ringbuffer
{
    uint8_t *buffer;
    uint16_t head;
    uint16_t tail;
    uint16_t capacity;
    bool isfull;
} RingBuffer_t;


void RB_Init(RingBuffer_t *rb, uint8_t *buf, uint16_t capacity);
bool RB_WriteBytes(RingBuffer_t *rb, uint8_t data);
uint8_t RB_ReadBytes(RingBuffer_t *rb);
uint16_t RB_GetLen(RingBuffer_t *rb);
void RB_Flush(RingBuffer_t *rb);

#endif