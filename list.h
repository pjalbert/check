#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef __List_h__
#define __List_h__
#define QSortLimit 10

typedef struct __Chaine {
    void* Member;
    struct __Chaine* Previous;
    struct __Chaine* Next;
} Chaine;

typedef int (CmpFct)(void *, void*);
typedef void (FreeFct)(void *);

typedef struct __List {
    int Count;
    Chaine* Begin;
    Chaine* Main;
    Chaine* End;
    CmpFct* pCmpFunction;
    FreeFct* pFreeFunction;
} List;

Chaine* InitChaine();
List* InitList();

extern void setFreeFuntion(List** ppList, FreeFct* pFreeFunction );
extern void setCmpFuntion(List** ppList, CmpFct* pCmpFunction );

extern int AddMemberOnTop(List ** ppList, void* pMember);
extern int AddMemberOnBottom(List ** ppList, void* pMember);

extern int SimpleRemoveMember(List ** ppList, void* pMember);

extern void* RemoveMemberFromTop(List ** ppList);
extern void* RemoveMemberFromBottom(List ** ppList);
extern void CleanList(List* ppList);
extern void FreeList(List* ppList);

extern void* SearchInList(List* pList, void* pKey);
extern void* SearchInSortedList(List* pList, void* pKey);

extern void SortList(List** ppList);

void* ReturnMemberAtIndex(List* pList, int Position);
void* ReturnChaineAtIndex(List* pList, int Position);

void __SortList(List** ppList, int g, int d);
void __SimpleSort(List** ppList, int g, int d);
int PartitionList(List** ppList, int g, int d);
void ExchangePosition(List** ppList, int i, int j) ;

#endif
