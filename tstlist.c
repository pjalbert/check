#include "list.h"

int main (int argc, const char * argv[]) {
    List* pList = NULL;
    char* pTmp;
    char* pMem_0;
    char* pMem_1;
    List* plSave = NULL;
    Chaine* pChaine;
    int i;

    // insert code here...
    printf("Hello, World!\n");

    for (i = 20; i > 0; i--) {
        pTmp = (char*) malloc(16);
        memset(pTmp, 0, 16);

        if (i%2 == 0)
            sprintf(pTmp, "Ying-%d", i);
        else
            sprintf(pTmp, "Yand-%d", i);

        if  (i == 1)
            pMem_0 = pTmp;

        if  (i == 8)
            pMem_1 = pTmp;

        AddMemberOnTop(&pList, pTmp);
    }

    for (i = 0; i < pList->Count; i++) {
        printf("Position %d : %s\n", i, (char*)ReturnMemberAtIndex(pList, i));
    }

    printf("+++++++++++++++\n");

    SortList(&pList);

    for (i = 0; i < pList->Count; i++) {
        printf("Position %d : %s\n", i, (char*)ReturnMemberAtIndex(pList, i));
    }

    printf("Search : %s\n", (char*)SearchInSortedList(pList, "Yand-5"));

    printf("+++++++++++++++\n");

    SimpleRemoveMember(&pList, pMem_0);
    free(pMem_0);

    printf("++++++++++\n");

    SimpleRemoveMember(&pList, pMem_1);
    free(pMem_1);

    for (i = 0; i < pList->Count; i++) {
        printf("Position 0 : %s\n", i, (char*)ReturnMemberAtIndex(pList, i));
    }

    printf("+++++++++++++++\n");


    CleanList(pList);

    return 0;
}
