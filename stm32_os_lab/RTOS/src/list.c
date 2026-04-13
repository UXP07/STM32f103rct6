#include "list.h"

/**
 * @brief: 初始化节点
 * @param: &(ListItem_t)
 */
void vListInitialiseItem(ListItem_t * const pxItem)
{
    pxItem->pvContainer = NULL;
}

/**
 * @brief: 初始化表头
 * @param: &(List_t)
 */
void vListInitalise(List_t * const pxList)
{
    pxList->pxIndex = (ListItem_t*) &(pxList->xListEnd);
    pxList->xListEnd.xItemValue = portMAX_DELAY;
    pxList->xListEnd.pxNext = (ListItem_t*) & (pxList->xListEnd);
    pxList->xListEnd.pxPrevious = (ListItem_t*) & (pxList->xListEnd);
    pxList->xListEnd.pvContainer = pxList;
    pxList->uxNumberOfItems = (UBaseType_t) 0U;
}

/**
 * @brief: 将新节点插入链表的尾部
 * @param: pxList: 表头 ;  pxNewListItem: 节点
 */
void vListInsertEnd(List_t * const pxList, ListItem_t * const pxNewListItem)
{
    ListItem_t * const pxIndex = pxList->pxIndex;
    pxNewListItem->pxNext = pxIndex;
    pxNewListItem->pxPrevious = pxIndex->pxPrevious;
    pxIndex->pxPrevious->pxNext = pxNewListItem;
    pxIndex->pxPrevious = pxNewListItem;
    pxNewListItem->pvContainer = pxList;
    pxList->uxNumberOfItems = (UBaseType_t) (pxList->uxNumberOfItems + 1U);
}

/**
 * @brief: 将新节点按顺序插入链表
 * @param: pxList: 表头 ;  pxNewListItem: 节点
 */
void vListInsert(List_t * const pxList, ListItem_t * const pxNewListItem)
{
    ListItem_t *pxIterator;
    const TickType_t xValueOfInsertion = pxNewListItem->xItemValue;
    if(xValueOfInsertion == portMAX_DELAY)
    {
        pxIterator = pxList->xListEnd.pxPrevious;
    }
    else
    {
        for(pxIterator = (ListItem_t*)&(pxList->xListEnd); pxIterator->pxNext->xItemValue <= xValueOfInsertion; pxIterator = pxIterator->pxNext)
        {
        }
    }
    pxNewListItem->pxNext = pxIterator->pxNext;
    pxNewListItem->pxPrevious = pxIterator;
    pxIterator->pxNext->pxPrevious = pxNewListItem;
    pxIterator->pxNext = pxNewListItem;
    pxNewListItem->pvContainer = pxList;
    pxList->uxNumberOfItems = (UBaseType_t)(pxList->uxNumberOfItems + 1U);
}

UBaseType_t uxListRemove(ListItem_t * const pxItemToRemove)
{
    List_t * const pxList = (List_t *)pxItemToRemove->pvContainer;
    pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;
    pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;

    if(pxList->pxIndex == pxItemToRemove)
    {
        pxList->pxIndex = pxItemToRemove->pxPrevious;
    }

    pxItemToRemove->pvContainer = NULL;
    pxList->uxNumberOfItems = (UBaseType_t)(pxList->uxNumberOfItems - 1U);
    return pxList->uxNumberOfItems;
}
