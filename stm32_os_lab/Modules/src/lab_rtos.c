#if 0
void test01(void)
{
    
    struct xLIST List_Test;
    struct xLIST_ITEM List_Item1;
    struct xLIST_ITEM List_Item2;
    struct xLIST_ITEM List_Item3;


    vListInitalise(&List_Test);
    
    vListInitialiseItem(&List_Item1);
    vListInitialiseItem(&List_Item2);
    vListInitialiseItem(&List_Item3);

    List_Item1.xItemValue = 1;
    List_Item2.xItemValue = 2;
    List_Item3.xItemValue = 3;

    vListInsert(&List_Test, &List_Item1);
    vListInsert(&List_Test, &List_Item2);
    vListInsert(&List_Test, &List_Item3);

    for(List_Test.pxIndex = List_Test.xListEnd.pxNext; List_Test.pxIndex!=&(List_Test.xListEnd); List_Test.pxIndex = List_Test.pxIndex->pxNext)
    {
        TickType_t temp = List_Test.pxIndex->xItemValue;
        printf("%ld\n", temp);
    }
    printf("There has %ld nodes\n", List_Test.uxNumberOfItems);

    for(List_Test.pxIndex = List_Test.xListEnd.pxNext; List_Test.pxIndex!=&(List_Test.xListEnd); List_Test.pxIndex = List_Test.pxIndex->pxNext)
    {
        uxListRemove(List_Test.pxIndex);
    }
    printf("There has %ld nodes after delete\n", List_Test.uxNumberOfItems);
}

#endif