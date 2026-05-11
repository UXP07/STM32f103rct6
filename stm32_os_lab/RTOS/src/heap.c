#include "portable.h"

#define heapSIZE_MAX                          (~((size_t)0))
#define heapADD_WILL_OVERFLOW(a, b)           ((a) > heapSIZE_MAX-(b))
#define heapBITS_PER_BYTE                     ((size_t) 8)
#define heapBLOCK_ALLOCATED_BITMASK           (((size_t)1) << ((sizeof(size_t) * heapBITS_PER_BYTE) - 1))
#define heapBLOCK_SIZE_IS_VALID(xBlockSize)   (((xBlockSize) & heapBLOCK_ALLOCATED_BITMASK) == 0)
#define heapALLOCATE_BLOCK(pxBlock)           ((pxBlock->xBlockSize) |= heapBLOCK_ALLOCATED_BITMASK)
#define heapBLOCK_IS_ALLOCATED(pxBlock)       (((pxBlock->xBlockSize) & heapBLOCK_ALLOCATED_BITMASK) != 0)
#define heapFREE_BLOCK(pxBlock)               ((pxBlock->xBlockSize) &= ~heapBLOCK_ALLOCATED_BITMASK)


#define heapPROTECT_BLOCK_POINTER(pxBlock)      (pxBlock)
#define heapVALIDATE_BLOCK_POINTER(pxBlock)     \
            configASSERT(((uint8_t*)(pxBlock) >= &(ucHeap[0])) && ((uint8_t *)(pxBlock) <= &(ucHeap[configTOTAL_HEAP_SIZE-1])))

#define heapSUBTRACT_WILL_UNDERFLOW(a, b)   ((a) < (b))
#define heapMINIMUM_BLOCK_SIZE                  (xHeapStructSize<<1)

#if (configAPPLICATION_ALLOCATED_HEAP == 1)
    extern uint8_t ucHeap[configTOTAL_HEAP_SIZE];
#else
    static uint8_t ucHeap[configTOTAL_HEAP_SIZE];
#endif

typedef struct A_BLOCK_LINK
{
    struct A_BLOCK_LINK * pxNextFreeBlock;
    size_t xBlockSize;
} BlockLink_t;

static const size_t xHeapStructSize = (sizeof(BlockLink_t) + ((size_t)(portBYTE_ALIGNMENT-1))) & ~((size_t)(portBYTE_ALIGNMENT_MASK));

static BlockLink_t xStart;
static BlockLink_t * pxEnd = NULL;

static size_t xFreeBytesRemaining  =(size_t) 0U;
static size_t xMinimumEverFreeBytesRemaining = (size_t) 0U;
static size_t xNumberOfSuccessfulAllocations = (size_t) 0U;
static size_t xNumberOfSuccessfulFrees = (size_t) 0U;


static void prvHeapInit(void)
{
    BlockLink_t * pxFirstFreeBlock;
    portPOINTER_SIZE_TYPE uxStartAddress, uxEndAddress;
    size_t xTotalHeapSize = configTOTAL_HEAP_SIZE;

    uxStartAddress = (portPOINTER_SIZE_TYPE) ucHeap;

    if((uxStartAddress & portBYTE_ALIGNMENT_MASK) != 0)
    {
        uxStartAddress += (portBYTE_ALIGNMENT - 1);
        uxStartAddress &= ~((portPOINTER_SIZE_TYPE) portBYTE_ALIGNMENT_MASK);
        xTotalHeapSize -= (size_t)(uxStartAddress - (portPOINTER_SIZE_TYPE)ucHeap);
    }

    xStart.pxNextFreeBlock = (void*) uxStartAddress;
    xStart.xBlockSize = (size_t) 0;

    uxEndAddress = uxStartAddress + (portPOINTER_SIZE_TYPE)xTotalHeapSize;
    uxEndAddress -= (portPOINTER_SIZE_TYPE) xHeapStructSize;
    uxEndAddress &= ~((portPOINTER_SIZE_TYPE) portBYTE_ALIGNMENT_MASK);

    pxEnd = (BlockLink_t *)uxEndAddress;
    pxEnd->xBlockSize = 0;
    pxEnd->pxNextFreeBlock = NULL;

    pxFirstFreeBlock = (BlockLink_t *)uxStartAddress;
    pxFirstFreeBlock->xBlockSize = (size_t)(uxEndAddress - (portPOINTER_SIZE_TYPE)pxFirstFreeBlock);
    pxFirstFreeBlock->pxNextFreeBlock = pxEnd;

    xMinimumEverFreeBytesRemaining = pxFirstFreeBlock->xBlockSize;
    xFreeBytesRemaining = pxFirstFreeBlock->xBlockSize;
}

static void prvInsertBlockIntoFreeList(BlockLink_t * pxBlockToInsert)
{
    BlockLink_t * pxIterator;
    uint8_t * puc;

    for(pxIterator = &xStart; pxIterator->pxNextFreeBlock < pxBlockToInsert; pxIterator = pxIterator->pxNextFreeBlock)
    {
    }

    if(pxIterator != &xStart)
    {
        heapVALIDATE_BLOCK_POINTER(pxIterator);
    }
    else mtCOVERAGE_TEST_MARKER();

    puc = (uint8_t *) pxIterator;
    if((puc + pxIterator->xBlockSize) == (uint8_t*) pxBlockToInsert)
    {
        pxIterator->xBlockSize += pxBlockToInsert->xBlockSize;
        pxBlockToInsert = pxIterator;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    puc = (uint8_t*) pxBlockToInsert;

    if((puc + pxBlockToInsert->xBlockSize) == (uint8_t*)(pxIterator->pxNextFreeBlock))
    {
        if(pxIterator->pxNextFreeBlock != pxEnd)
        {
            pxBlockToInsert->xBlockSize += pxIterator->pxNextFreeBlock->xBlockSize;
            pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock->pxNextFreeBlock;
        }
        else
        {
            pxBlockToInsert->pxNextFreeBlock = pxEnd;
        }
    }
    else
    {
        pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;
    }

    if(pxIterator != pxBlockToInsert)
    {
        pxIterator->pxNextFreeBlock = pxBlockToInsert;
    }
    else mtCOVERAGE_TEST_MARKER();

}


void * pvPortMalloc(size_t xWantedSize)
{
    BlockLink_t * pxBlock;
    BlockLink_t * pxPreviousBlock;
    BlockLink_t * pxNewBlockLink;
    void * pvReturn = NULL;
    size_t xAdditionalRequiredSize;

    if(xWantedSize > 0)
    {
        if(heapADD_WILL_OVERFLOW(xWantedSize, xHeapStructSize) == 0)
        {
            xWantedSize += xHeapStructSize;
            if((xWantedSize & portBYTE_ALIGNMENT_MASK) != 0x00)
            {
                xAdditionalRequiredSize = portBYTE_ALIGNMENT - (xWantedSize & portBYTE_ALIGNMENT_MASK);
                if(heapADD_WILL_OVERFLOW(xWantedSize, xAdditionalRequiredSize) == 0)
                {
                    xWantedSize += xAdditionalRequiredSize;
                }
                else
                {
                    xWantedSize = 0;
                }
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        else
        {
            xWantedSize = 0;
        }
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }
    
    vTaskSuspendAll();
    {
        if(pxEnd == NULL)
        {
            prvHeapInit();
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }

        if(heapBLOCK_SIZE_IS_VALID(xWantedSize) != 0)   // 合法
        {
            if((xWantedSize > 0) && (xWantedSize <= xFreeBytesRemaining))
            {
                pxPreviousBlock = &xStart;
                pxBlock = heapPROTECT_BLOCK_POINTER(xStart.pxNextFreeBlock);
                heapVALIDATE_BLOCK_POINTER(pxBlock);

                while ((pxBlock->xBlockSize < xWantedSize) && (pxBlock->pxNextFreeBlock != NULL))
                {
                    pxPreviousBlock = pxBlock;
                    pxBlock = pxBlock->pxNextFreeBlock;
                    heapVALIDATE_BLOCK_POINTER(pxBlock);
                }
                
                if(pxBlock != pxEnd)
                {
                    pvReturn = (void*)((uint8_t*)(pxPreviousBlock->pxNextFreeBlock) + xHeapStructSize);
                    heapVALIDATE_BLOCK_POINTER(pvReturn);
                    
                    pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;
                    configASSERT(heapSUBTRACT_WILL_UNDERFLOW(pxBlock->xBlockSize, xWantedSize) == 0)

                    if((pxBlock->xBlockSize - xWantedSize) > heapMINIMUM_BLOCK_SIZE)
                    {
                        pxNewBlockLink = (void*)(((uint8_t*)pxBlock) + xWantedSize);
                        configASSERT(((size_t)pxNewBlockLink & portBYTE_ALIGNMENT_MASK) == 0);

                        pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
                        pxBlock->xBlockSize = xWantedSize;

                        pxNewBlockLink->pxNextFreeBlock = pxPreviousBlock->pxNextFreeBlock;
                        pxPreviousBlock->pxNextFreeBlock = pxNewBlockLink;

                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }

                    xFreeBytesRemaining -= pxBlock->xBlockSize;
                    if(xFreeBytesRemaining < xMinimumEverFreeBytesRemaining)
                    {
                        xMinimumEverFreeBytesRemaining = xFreeBytesRemaining;
                    }
                    else mtCOVERAGE_TEST_MARKER();

                    heapALLOCATE_BLOCK(pxBlock);
                    pxBlock->pxNextFreeBlock = NULL;
                    xNumberOfSuccessfulAllocations++;

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
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }

    }
    xTaskResumeAll();

    return pvReturn;
    
}

void vPortFree(void * pv)
{
    uint8_t * puc = (uint8_t*) pv;
    BlockLink_t * pxLink;

    if(pv != NULL)
    {
        puc -= xHeapStructSize;
        pxLink = (void *) puc;

        heapVALIDATE_BLOCK_POINTER(pxLink);
        // configASSERT(heapBLOCK_IS_ALLOCATED(pxLink) != 0);
        // configASSERT(pxLink->pxNextFreeBlock == NULL);
        if(heapBLOCK_IS_ALLOCATED(pxLink) != 0)
        {
            if(pxLink->pxNextFreeBlock == NULL)
            {
                heapFREE_BLOCK(pxLink);

                vTaskSuspendAll();
                {
                    xFreeBytesRemaining += pxLink->xBlockSize;
                    prvInsertBlockIntoFreeList(pxLink);
                    xNumberOfSuccessfulFrees++;
                }
                xTaskResumeAll();
            }
            else mtCOVERAGE_TEST_MARKER();
        }
        else mtCOVERAGE_TEST_MARKER();
    }

    return ;
}