#ifndef LIST_H
#define LIST_H

#include "portmacro.h"

// struct xMINI_LIST_ITEM
// {
//     TickType_t xItemValue;
//     struct xLIST_ITEM *pxNext;
//     struct xLIST_ITEM *pxPrevious;
// };
// typedef struct xMINI_LIST_ITEM MiniListItem_t;

struct xLIST_ITEM
{
    TickType_t xItemValue;
    struct xLIST_ITEM *pxNext;
    struct xLIST_ITEM *pxPrevious;
    void *pvOwner;
    struct xLIST *pvContainer;
};
typedef struct xLIST_ITEM ListItem_t;

typedef struct xLIST
{
    UBaseType_t uxNumberOfItems;
    ListItem_t *pxIndex;
//     MiniListItem_t xListEnd;
    ListItem_t xListEnd;
} List_t;
typedef struct xLIST List_t;

#define listSET_LIST_ITEM_OWNER(pxListItem, pxOwner) \
        ((pxListItem)->pvOwner = (void*)(pxOwner))

#define listGET_LIST_ITEM_OWNER(pxListItem) \
        ((pxListItem)->pvOwner)

#define listSET_LIST_ITEM_VALUE(pxListItem, xValue) \
        ((pxListItem)->xItemValue = (xValue))

#define listGET_LIST_ITEM_VALUE(pxListItem) \
        ((pxListItem)->xItemValue)

/* 获取链表根节点的节点计数器的值 */
#define listGET_ITEM_VALUE_OF_HEAD_ENTRY( pxList )\
        ( ( ( pxList )->xListEnd ).pxNext->xItemValue )

/* 获取链表的入口节点 */
#define listGET_HEAD_ENTRY( pxList )\
        ( ( ( pxList )->xListEnd ).pxNext )

/* 获取节点的下一个节点 */
#define listGET_NEXT( pxListItem )\
        ( ( pxListItem )->pxNext )

/* 获取链表的最后一个节点 */
#define listGET_END_MARKER( pxList )\
        ( ( ListItem_t const *) ( &( ( pxList )->xListEnd ) ) )

/* 判断链表是否为空 */
#define listLIST_IS_EMPTY( pxList )\
        ( ( ( pxList )->uxNumberOfItems == ( UBaseType_t )0 ) ? pdTRUE : pdFALSE )

/* 获取链表的节点数 */
#define listCURRENT_LIST_LENGTH( pxList )\
        ( ( pxList )->uxNumberOfItems )

#define listLIST_ITEM_CONTAINER(pxListItem)     \
        ((pxListItem)->pvContainer)

#define listGET_OWNER_OF_HEAD_ENTRY(pxList) \
        ((&((pxList)->xListEnd))->pxNext->pvOwner)

#define listIS_CONTAINED_WITHIN(pxList, pxListItem)     (((pxListItem)->pvContainer == (pxList)) ? pdTRUE : pdFALSE)

/* 获取链表第一个节点的OWNER，即TCB */
#define listGET_OWNER_OF_NEXT_ENTRY( pxTCB, pxList )                                       \
do{                                                                                        \
    List_t * const pxConstList = ( pxList );                                               \
    /* Increment the index to the next item and return the item, ensuring */               \
    /* we don't return the marker used at the end of the list.  */                         \
    ( pxConstList )->pxIndex = ( pxConstList )->pxIndex->pxNext;                           \
    if( ( void * ) ( pxConstList )->pxIndex == ( void * ) &( ( pxConstList )->xListEnd ) ) \
    {                                                                                      \
        ( pxConstList )->pxIndex = ( pxConstList )->xListEnd.pxNext;                       \
    }                                                                                      \
    ( pxTCB ) = ( pxConstList )->pxIndex->pvOwner;                                         \
}while(0)

#define listINSERT_END(pxList, pxNewListItem)                                              \
        do{                                                                                \
                ListItem_t * const pxIndex = (pxList)->pxIndex;                            \
                (pxNewListItem)->pxNext = pxIndex;                                         \
                (pxNewListItem)->pxPrevious = pxIndex->pxPrevious;                         \
                pxIndex->pxPrevious->pxNext = (pxNewListItem);                             \
                pxIndex->pxPrevious = (pxNewListItem);                                     \
                (pxNewListItem)->pvContainer = (pxList);                                   \
                ((pxList)->uxNumberOfItems) = (UBaseType_t) (((pxList)->uxNumberOfItems) + 1U); \
        }while(0)

#define listREMOVE_ITEM(pxItemToRemove)                                                                       \
        do{                                                                                     \
                List_t * const pxList = (pxItemToRemove)->pvContainer;                          \
                (pxItemToRemove)->pxPrevious->pxNext = (pxItemToRemove)->pxNext;                \
                (pxItemToRemove)->pxNext->pxPrevious = (pxItemToRemove)->pxPrevious;            \
                if(pxList->pxIndex == (pxItemToRemove))                                         \
                {                                                                               \
                        pxList->pxIndex = (pxItemToRemove)->pxPrevious;                         \
                }                                                                               \
                (pxItemToRemove)->pvContainer = NULL;                                           \
                ((pxList)->uxNumberOfItems) = (UBaseType_t) (((pxList)->uxNumberOfItems) - 1U); \
        }while(0)




void vListInitialiseItem(ListItem_t * const pxItem);
void vListInitalise(List_t * const pxList);
void vListInsertEnd(List_t * const pxList, ListItem_t * const pxNewListItem);
void vListInsert(List_t * const pxList, ListItem_t * const pxNewListItem);
UBaseType_t uxListRemove(ListItem_t * const pxItemToRemove);

#endif