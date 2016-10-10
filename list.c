/*
 *  list.c
 *  Idiome
 *
 *  Created by Pierre-Jean ALBERT on Fri Nov 08 2002.
 *  Copyright (c) 2002 __The Intellect Projet__. All rights reserved.
 *
 */
#include "list.h"

Chaine* InitChaine() {
    Chaine* pChaine;

    pChaine = malloc (sizeof(Chaine));

    pChaine->Previous = NULL;
    pChaine->Next = NULL;

    return pChaine;
}

List* InitList() {
    List* pList;

    pList = malloc (sizeof(List));

    pList->Count = 0;
    pList->Begin = NULL;
    pList->Main = NULL;
    pList->End = NULL;

    pList->pCmpFunction = (CmpFct *)strcmp;
    pList->pFreeFunction = (FreeFct *)free;

    return pList;
}

void setFreeFuntion(List** ppList, FreeFct* pFreeFunction) {

    if (pFreeFunction)
        (*ppList)->pFreeFunction = pFreeFunction;
}

void setCmpFuntion(List** ppList, CmpFct* pCmpFunction) {

    if (pCmpFunction)
        (*ppList)->pCmpFunction = pCmpFunction;
}

int AddMemberOnTop(List ** ppList, void* pMember) {
    Chaine* pChaine;

    if (!*ppList || (*ppList)->Count == 0) {
        if (!*ppList)
            *ppList = InitList();
        pChaine = InitChaine();
        pChaine->Member = pMember;

        (*ppList)->Count++;
        (*ppList)->Begin = pChaine;
        (*ppList)->Main = pChaine;
        (*ppList)->End = pChaine;
    }
    else {
        pChaine = InitChaine();
        pChaine->Member = pMember;
        (*ppList)->Main->Previous = pChaine;
        pChaine->Next = (*ppList)->Main;

        (*ppList)->Count++;
        (*ppList)->Main = pChaine;
        (*ppList)->Begin = pChaine;
    }

    return 1;
}

int AddMemberOnBottom(List ** ppList, void* pMember) {
    Chaine* pChaine;

    if (!*ppList || (*ppList)->Count == 0) {
        if (!*ppList)
            *ppList = InitList();
        pChaine = InitChaine();
        pChaine->Member = pMember;

        (*ppList)->Count++;
        (*ppList)->Begin = pChaine;
        (*ppList)->Main = pChaine;
        (*ppList)->End = pChaine;
    }
    else {
        pChaine = InitChaine();
        pChaine->Member = pMember;
        pChaine->Previous = (*ppList)->End;

        (*ppList)->Count++;
        (*ppList)->End->Next = pChaine;
        (*ppList)->End = pChaine;
    }

    return 1;
}

int SimpleRemoveMember(List ** ppList, void* pMember) {
    Chaine* pChaine;

    if (!*ppList || (*ppList)->Count == 0)
        return 1;

    pChaine = (*ppList)->Main;
    while (pChaine) {

        if (pChaine->Member == pMember) {

            (*ppList)->Count--;

            if ((*ppList)->Count > 0) {
                if (pChaine->Previous)
                    (pChaine->Previous)->Next = (pChaine->Next);
                else {
                    (*ppList)->Begin = (*ppList)->Begin->Next;
                    (*ppList)->Begin->Previous = NULL;
                    (*ppList)->Main = (*ppList)->Begin;
                }

                if (pChaine->Next) {
                    (pChaine->Next)->Previous = (pChaine->Previous);
                }
                else {
                    (*ppList)->End = (*ppList)->End->Previous;
                    (*ppList)->End->Next = NULL;
                }
            }
            else {
                (*ppList)->Begin = NULL;
                (*ppList)->Main = NULL;
                (*ppList)->End = NULL;
            }

            free(pChaine);
            return 1;
        }

        pChaine = pChaine->Next;
    }

   return 0;
}


void* RemoveMemberFromTop(List** ppList) {
    void* pMember;

    if (!*ppList || (*ppList)->Count == 0)
        return NULL;

    pMember = (*ppList)->Begin->Member;
    (*ppList)->Begin = (*ppList)->Begin->Next;

    free((*ppList)->Main);
    if ((*ppList)->Begin)
        (*ppList)->Begin->Previous = NULL;

    (*ppList)->Main = (*ppList)->Begin;
    (*ppList)->Count--;

    if ((*ppList)->Count == 0)
        (*ppList)->End = (*ppList)->Begin;

    return pMember;
}

void* RemoveMemberFromBottom(List** ppList) {
    Chaine* pChaine;
    void* pMember;

    if (!*ppList || (*ppList)->Count == 0)
        return NULL;

    pMember = (*ppList)->End->Member;
    pChaine = (*ppList)->End;
    (*ppList)->End = (*ppList)->End->Previous;

    free(pChaine);
    if ((*ppList)->End)
        (*ppList)->End->Next = NULL;

    (*ppList)->Count--;
    if ((*ppList)->Count == 0) {
        (*ppList)->Begin = (*ppList)->End;
        (*ppList)->Main = (*ppList)->End;
    }

    return pMember;
}

void FreeList(List* ppList) {
    void* pTmp;

    if (!ppList || ppList->Count == 0)
        return ;

    while(ppList->Count > 0) {
        pTmp = (void*) RemoveMemberFromBottom(&ppList);
        ppList->pFreeFunction(pTmp);
    }

    free(ppList);
}

void CleanList(List* ppList) {

    if (!ppList || (ppList)->Count == 0)
        return ;

    while(ppList->Count > 0) {
        RemoveMemberFromBottom(&ppList);
    }

    free(ppList);
}

void* SearchInList(List* pList, void* pKey ) {
    Chaine* pChaine;

    pChaine = pList->Main;
    while (pChaine) {
        if ((pList->pCmpFunction(pKey, pChaine->Member)) == 0)
            return pChaine->Member;
        pChaine = pChaine->Next;
    }

    if ((pList->pCmpFunction(pKey, pChaine->Member)) == 0)
        return pChaine->Member;

    return NULL;
}


void* SearchInSortedList(List* pList, void* pKey ) {
    Chaine* pChaine;
    int Low, Mid, High;
    int rc;

    Low = 0;
    High = pList->Count-1;
    pChaine = pList->Main;

    while (Low <= High) {
        Mid = (Low + High)/2;
        rc = (pList->pCmpFunction(pKey, ReturnMemberAtIndex(pList, Mid)));
        if (rc == 0)
            return ReturnMemberAtIndex(pList, Mid);
        if (rc < 0)
            High = Mid-1;
        else
            Low = Mid+1;
    }

    return NULL;
}


void* ReturnMemberAtIndex(List* pList, int Position) {
    Chaine* pChaine;
    int pos;

    if (pList->Count <= Position)
        return NULL;

    pChaine = pList->Main;
    pos = 0;
    while (pos < Position) {
        pos++;
        pChaine = pChaine->Next;
    }

    return pChaine->Member;
}


void* ReturnChaineAtIndex(List* pList, int Position) {
    Chaine* pChaine;
    int pos;

    if (pList->Count <= Position)
        return NULL;

    pChaine = pList->Main;
    pos = 0;
    while (pos < Position) {
        pos++;
        pChaine = pChaine->Next;
    }

    return pChaine;
}


void SortList(List** ppList) {
    __SortList(ppList, 0, (*ppList)->Count-1);
}


void __SortList(List** ppList, int lower, int upper) {
    int	i, m;
    void *pivot;

    if (lower + QSortLimit > upper)
        __SimpleSort(ppList, lower, upper);
    else {
        if (lower < upper) {
            ExchangePosition(ppList, lower, (upper+lower)/2);
            pivot = ReturnMemberAtIndex((*ppList), lower);
            m = lower;
            for (i = lower + 1; i <= upper; i++) {
                if ((*ppList)->pCmpFunction(ReturnMemberAtIndex((*ppList), i), pivot) < 0) {
                    m++;
                    ExchangePosition(ppList, m, i);
                }
            }
            ExchangePosition(ppList, lower, m);
            __SortList (ppList, lower, m - 1);
            __SortList (ppList, m + 1, upper);
        }
    }
}


void __SimpleSort(List** ppList, int lower, int upper) {
    int i, j;

    for(i = lower; i < upper+1; i++) {
        for (j = i+1; j < upper+1; j++) {
            if ((*ppList)->pCmpFunction(ReturnMemberAtIndex((*ppList), i),
                                        ReturnMemberAtIndex((*ppList), j)) > 0)
                ExchangePosition(ppList, i, j);
        }
    }
}


void ExchangePosition(List** ppList, int i, int j) {
    Chaine *pChaine1;
    Chaine *pChaine2;
    Chaine *pCTmp;
    void* pMember;

    pChaine1 = ReturnChaineAtIndex((*ppList), i);
    pChaine2 = ReturnChaineAtIndex((*ppList), j);

    if (!pChaine1 || !pChaine2)
        return;

    pMember = pChaine1->Member;
    pChaine1->Member = pChaine2->Member;
    pChaine2->Member = pMember;
}

