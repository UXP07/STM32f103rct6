#include "lab_usart_ringbuffer.h"

void RB_Init(RingBuffer_t *rb, uint8_t *buf, uint16_t capacity)
{
    rb->buffer = buf;
    rb->capacity = capacity;
    rb->head = 0;
    rb->tail = 0;
    rb->isfull = false;
}


bool RB_WriteBytes(RingBuffer_t *rb, uint8_t data)
{

    if(rb->isfull == false)
    {
        return false;
    }

    rb->buffer[rb->tail] = data;
    rb->tail = (rb->tail+1)%rb->capacity;

    if(rb->tail == rb->head)
    {
        rb->isfull = true;
    }

    return true;
}


uint8_t RB_ReadBytes(RingBuffer_t *rb)
{
    if(RB_GetLen(rb) == 0)
    {
        return false;
    }

    uint8_t data = rb->buffer[rb->head];
    rb->head = (rb->head + 1)%rb->capacity;
    rb->isfull = false;

    return data;
}


uint16_t RB_GetLen(RingBuffer_t *rb)
{
    uint16_t len = 0;
    if(rb->tail > rb->head)
    {
        len = rb->tail - rb->head;
    }
    else
    {
        len = rb->capacity - rb->head + rb->tail;
    }
    return len;
}


void RB_Flush(RingBuffer_t *rb)
{
    rb->head = 0;
    rb->tail = 0;
    rb->isfull  =false;
}


