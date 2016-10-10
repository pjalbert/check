#include "thread.h"

/**
    Thread Managment function */
Thread* InitThread(void* pThreadData, char *Name, unsigned long Id, FreeFct* pFreeDataFunction)
{
    Thread* pThread;

    pThread = malloc(sizeof(Thread));
    if (!pThread)
    	return NULL;

    pThread->Id = Id;

    pThread->pTaskManager = NULL;
    pThread->pSynchZone = NULL;

    pThread->Silent = true;
    pThread->RunningState = false;

    if (!Name) {
        pThread->Name = (char*) malloc (strlen (DEFAULT_THREAD_NAME)*sizeof(char) + 1);
        memset(pThread->Name, 0, strlen(DEFAULT_THREAD_NAME)*sizeof(char) + 1);
        strcpy(pThread->Name, DEFAULT_THREAD_NAME);
    }
    else {
      pThread->Name = (char*) malloc (strlen(Name)*sizeof(char) + 1);
      memset(pThread->Name, 0, strlen(Name)*sizeof(char) + 1);
      strcpy(pThread->Name, Name);
    }

    if (!pThreadData)
        pThread->pThreadData = NULL;
    else
        pThread->pThreadData = pThreadData;

    pThread->pBeginningThread = (ThreadBeginning *)DefaultBeginning;
    pThread->pRunningThread = (ThreadRunning *)DefaultRunning;
    pThread->pEndingThread = (ThreadEnding *)DefaultEnding;

    pThread->pRunCondition = (ThreadRunCondition *)DefaultRunCondition;
    pThread->pRunSharedZone = (ThreadRunShared *)DefaultRunSharedZone;

    if (pFreeDataFunction)
        pThread->pFreeDataFunction = (FreeFct*)pFreeDataFunction;
    else
        pThread->pFreeDataFunction = NULL;

    pthread_mutex_init(&pThread->Locked.mutex, NULL);
    pthread_cond_init(&pThread->Locked.signal, NULL);

    printf("End of Thead Init %ld\n",  pThread->Id);

    return pThread;
}

void* GlobalThreadExecution(void* pVoid)
{
    Thread* pThread;
    SynchZone* pSZone;

    pThread = (Thread*) pVoid;

    pThread->pBeginningThread(pThread);

    while(pThread->pRunCondition(pThread)) {

        pThread->pRunningThread(pThread);

        if (pThread->RunningState) {

            if (pThread->pSynchZone)  {

                if (pThread->AskSmartLock) {
                    pSZone = (SynchZone*) pThread->pSynchZone;

                    if (AcquireSmartLock(pSZone)) {

                        if (pThread->pRunSharedZone) {
                            pThread->pRunSharedZone(pThread);
                            ReleaseLock(pSZone);
                            pThread->AskSmartLock = false;
                        }
                    }
                }

                if (pThread->AskLock) {
                    pSZone = (SynchZone*) pThread->pSynchZone;

                    if (AcquireLock(pSZone)) {

                        pThread->pRunSharedZone(pThread);
                        ReleaseLock(pSZone);
                        pThread->AskLock = false;
                    }
                }
            }
        }

        sched_yield();
    }

    pThread->pEndingThread(pThread);
}

int YieldThread(Thread* pThread) {
    int rc;

    pThread->Paused = true;

    rc = pthread_mutex_lock(&pThread->Locked.mutex);
    if (rc != 0)
        return 0;

    rc = pthread_cond_wait(&pThread->Locked.signal, &pThread->Locked.mutex);
    if (rc != 0)
        return 0;

    rc = pthread_mutex_unlock(&pThread->Locked.mutex);
    if (rc != 0)
        return 0;

    return 1;
}

int ResumeThread(Thread* pThread) {
    int rc;

    rc = pthread_mutex_lock(&pThread->Locked.mutex);
    if (rc != 0)
        return 0;

    rc = pthread_cond_signal(&pThread->Locked.signal);
    if (rc != 0)
        return 0;

    rc = pthread_mutex_unlock(&pThread->Locked.mutex);
    if (rc != 0)
        return 0;

    pThread->Paused = false;

    return 1;
}

void FreeThread(void* pMemory)
{
    Thread* pThread;


    pThread = (Thread*)pMemory;

    if (!pThread->Silent)
        printf("FreeThread %d\n", pThread->Id);

    if (pThread->pThreadData)
    	if (pThread->pFreeDataFunction) {
    	    if (!pThread->Silent)
                printf("FreeData in FreeThread %d\n", pThread->Id);
            pThread->pFreeDataFunction(pThread->pThreadData);
            pThread->pThreadData = NULL;
    }

    if (pThread->Name) {
        free(pThread->Name);
        pThread->Name = NULL;
    }

    if (pThread->pSynchZone) {
        if (!pThread->Silent)
            printf("Calling FreeSynchZone in FreeThread\n");
        FreeSynchZone(pThread->pSynchZone);
        pThread->pSynchZone= NULL;
    }

    printf("End of Thead Free %ld\n",  pThread->Id);

    free(pThread);
}

/**
    SynchZone Managment function */
SynchZone* InitSynchZone(Thread* pThread, void* pSData, FreeFct* pFreeDataFunction)
{
    SynchZone* pSZone;

    pSZone = malloc(sizeof(SynchZone));

    pSZone->Id = -1;

    pthread_mutex_init(&pSZone->Locked.mutex, NULL);
    pthread_cond_init(&pSZone->Locked.signal, NULL);

    if (!pSData)
        pSZone->pSharedData = NULL;
    else
        pSZone->pSharedData = pSData;

    if (!pThread)
        pSZone->plThreadInvolved = NULL;
    else {
        pSZone->plThreadInvolved = NULL;
        pThread->pSynchZone = pSZone;
        AddMemberOnTop(&pSZone->plThreadInvolved, pThread);
    }

    if (pFreeDataFunction)
        pSZone->pFreeDataFunction = (FreeFct*)pFreeDataFunction;
    else
        pSZone->pFreeDataFunction = (FreeFct*)free;

    return pSZone;
}

void FreeSynchZone(void* pMemory)
{
    SynchZone* pSynchZone;
    Thread* pThread;
    List* pList;
    Chaine* pChaine;

    pSynchZone = (SynchZone*)pMemory;

    if (pSynchZone->pSharedData)
        if (pSynchZone->pFreeDataFunction) {

            printf("FreeData in FreeSynchZone\n");
            pSynchZone->pFreeDataFunction(pSynchZone->pSharedData);
            pSynchZone->pSharedData = NULL;
    }

    pList = (List*)pSynchZone->plThreadInvolved;

    DetachThreadFromSynchZone(pSynchZone);

    FreeList(pList);

    pSynchZone->plThreadInvolved = NULL;

    free(pSynchZone);
}

int AddNewThreadInSynchZone(SynchZone* pSZone, Thread* pThread)
{
    if (!pThread)
        return 0;

    pThread->pSynchZone = pSZone;
    AddMemberOnTop(&pSZone->plThreadInvolved, pThread);

    return 1;
}

int DetachThreadFromSynchZone(SynchZone* pSynchZone) {
    Thread* pThread;
    List* pList;
    Chaine* pChaine;

    pList = (List*)pSynchZone->plThreadInvolved;
    pChaine = pList->Main;
    while (pChaine) {
        pThread = (Thread*)pChaine->Member;

        printf("DetachThread %s\n", pThread->Name);

        pThread->pSynchZone = NULL;;
        pChaine = pChaine->Next;
    }

    return 1;
}

/**
    TaskManager Managment function */
TaskManager* InitTaskManager(unsigned long Id) {
    TaskManager* pTaskManager;

    pTaskManager = malloc(sizeof(TaskManager));
    pTaskManager->plThread = NULL;
    pTaskManager->plSynchZone = NULL;

    pTaskManager->Id = Id;

    return pTaskManager;
}

void FreeTaskManager(TaskManager* pTaskManager, boolean iThread, boolean iSZone)
{
    List* pList;
    Thread* pThread;
    Chaine* pChaine;

    printf("FreeTaskManager Begin\n");

    pList = (List*)pTaskManager->plThread;
    pChaine = pList->Main;
    while (pChaine) {
      pThread = (Thread*)pChaine->Member;
      printf("TaskManager pThread-> %s\n", pThread->Name);
      pChaine = pChaine->Next;
    }


    if (iThread) {
        pList = (List*)pTaskManager->plThread;
        if (pList) {
            printf("TaskManager plThread CleanList\n");
            CleanList(pList);
            pTaskManager->plThread = NULL;
        }
    }

    if (iSZone) {
        pList = (List*)pTaskManager->plSynchZone;
        if (pList) {
            printf("TaskManager plSynchZone FreeList\n");
            FreeList(pList);
            pTaskManager->plSynchZone = NULL;
        }
    }

    free(pTaskManager);
}

int AddNewThread(TaskManager* pTaskManager, Thread* pThread)
{

    pThread->pTaskManager = pTaskManager;

    AddMemberOnTop(&pTaskManager->plThread, pThread);

    return 0;
}

int AddNewSynchZone(TaskManager* pTaskManager, SynchZone* pSZone)
{

    AddMemberOnTop(&pTaskManager->plSynchZone, pSZone);

    return 0;
}

int StartingTaskManager(TaskManager* pTaskManager) {
    List* pList;
    Chaine* pChaine;
    Thread* pThread;
    int rc;

    pList = pTaskManager->plThread;
    pChaine = pList->Main;
    rc = 0;
    while (pChaine) {
        pThread = (Thread*)pChaine->Member;

        if (!pThread->Silent)
            printf("Launching -> name '%s' Id %ld\n", pThread->Name, pThread->Id);
        pthread_create(&pThread->IdThread, NULL, GlobalThreadExecution, pThread);
        pChaine = pChaine->Next;
    }

    return rc;
}

int StartThread(TaskManager* pTaskManager, Thread* pThread)
{

    pThread->pTaskManager = pTaskManager;

    if (!pThread->Silent)
        printf("Launching -> name '%s' Id %ld\n", pThread->Name, pThread->Id);
    pthread_create(&pThread->IdThread, NULL, GlobalThreadExecution, pThread);

    AddMemberOnTop(&pTaskManager->plThread, pThread);
    return 0;
}

int WaitingTaskManager(TaskManager* pTaskManager) {
    Chaine* pChaine;
    Thread* pThread;
    int rc;

    pChaine = pTaskManager->plThread->Main;
    while (pChaine) {
        pThread = (Thread*)pChaine->Member;

        pthread_join( pThread->IdThread, NULL);
        pChaine = pChaine->Next;
    }

    return rc;
}


int AcquireSmartLock(SynchZone* pSZone)
{
    int rc;

    rc = pthread_mutex_trylock(&pSZone->Locked.mutex);
    if (rc == EBUSY)
        return 0;

    return 1;
}

int AcquireLock(SynchZone* pSZone)
{
    int rc;

    rc = pthread_mutex_lock(&pSZone->Locked.mutex);
    if (rc != 0)
        return 0;

    return 1;
}

int ReleaseLock(SynchZone* pSZone)
{
    int rc;

    rc = pthread_mutex_unlock(&pSZone->Locked.mutex);
    if (rc != 0)
        return 0;

    return 1;
}


/**
    Message Managment function */


/**
    Bus Managment function */
BusManager* InitBusManager(unsigned long Id, unsigned short NbPool, ) {
    BusManager* pBusManager;

    pBusManager = malloc(sizeof(BusManager));
    if (!pBusManager)
        return NULL;

    pBusManager->plThread = NULL;
    pBusManager->plData = NULL;

    pBusManager->Id = Id;
    pBusManager->NbPool = NbPool;

    pBusManager->Uuid = pUuid;

    pBusManager->pBeginningThread = NULL;;
    pBusManager->pRunningThread = NULL;;
    pBusManager->pEndingThread = NULL;

    return pBusManager;
}

int DispatchBusMessage(BusManager* pBusManager)
{
    List* pListData;
    List* pListThread;
    void* pMemory;

    pListData = pBusManager->plData;
    pListThread = pBusManager->plThread;

    while (pBusManager->RunningState) {

        while (pListData->Count) {

            pMemory = (void* )pBusManager->pGetMessage(pBusManager);
            if (!pMemory)
                continue;

            while (pListThread->Count  < pBusManager->NbPool) {
                pThread = (Thread*)InitThread(pMemory, "Pool Exec", -1, NULL);
                pThread->pBusManager = pBusManager;

                pThread->pBeginningThread = pBusManager->pBeginningThread;
                pThread->pRunningThread = pBusManager->pRunningThread;
                pThread->pEndingThread = pBusManager->pEndingThread;

                if (pThread) {
                    if (!pThread->Silent)
                        printf("Launching -> name '%s' Id %ld\n", pThread->Name, pThread->Id);
                    pthread_create(&pThread->IdThread, NULL, GlobalThreadExecution, pThread);

                    AddMemberOnTop(&pListThread, pThread);
                }
            }
        }

        // Wait Stuff TO DO
    }


    return 1;
}

void* GetMessage(BusManager* pBusManager) {



    return
}

/**
    Default thread managment function */
int DefaultBeginning(Thread* pThread)
{
    pThread->RunningState = true;

    if (!pThread->Silent)
        printf("Hi, i am just a starting thread %ld\n", (unsigned long)pThread->Id);

    return 1;
}

int DefaultRunning(Thread* pThread)
{

    if (!pThread->Silent)
        printf("Well, i am running thread %ld/%ld\n", (unsigned long)pThread->Id, pThread->RunningState);
    pThread->AskSmartLock = true;
    return 1;
}

int DefaultEnding(Thread* pThread)
{

    if (!pThread->Silent)
        printf("ok, i am an ending thread %ld\n", (unsigned long)pThread->Id);

    return 1;
}

/**
    Execute once     1 mean continue     0 tell stop     */
int DefaultRunCondition(Thread* pThread)
{

    if (pThread->RunningState)
	    return 1;
	  else
	  	return 0;
}

/**
    Each Thread is counting  */
int DefaultRunSharedZone(Thread* pThread)
{
    SynchZone* pSZone;
    int* Count;

    pSZone = (SynchZone*)pThread->pSynchZone;

    Count = (int*)pSZone->pSharedData;

    if (pThread->Name)
        printf("Th Name %s - %ld -> Cnt %d\n", pThread->Name, (unsigned long)pThread->Id, (*Count)++);
    else
        printf("Th <null> - %ld -> Cnt %d\n", (unsigned long)pThread->Id, (*Count)++);

    if ((int)(*Count) >= 100 )
        pThread->RunningState = false;

    return 1;
}

/**
    Default Bus built-in thread managment function */
int DefaultBeginning(Thread* pThread)
{
    pThread->RunningState = true;


    if (!pThread->Silent)
        printf("Hi, i am just a starting thread %ld\n", (unsigned long)pThread->Id);

    return 1;
}

int DefaultRunning(Thread* pThread)
{

    if (!pThread->Silent)
        printf("Well, i am running thread %ld/%ld\n", (unsigned long)pThread->Id, pThread->RunningState);
    pThread->AskSmartLock = true;
    return 1;
}

int DefaultEnding(Thread* pThread)
{

    if (!pThread->Silent)
        printf("ok, i am an ending thread %ld\n", (unsigned long)pThread->Id);

    return 1;
}

