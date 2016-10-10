#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include "list.h"
#include "archetyp.h"

#ifndef __Thread_h__
#define __Thread_h__

#define randomize(num) (int) ((float) (num) * rand () / (RAND_MAX + 1.0));

#define DEFAULT_THREAD_NAME  "Anonymous - Threading live"

typedef int (ThreadRunning)(void *);
typedef int (ThreadBeginning)(void *);
typedef int (ThreadEnding)(void *);

typedef int (ThreadRunCondition)(void*);

typedef int (ThreadRunShared)(void *);

typedef int (BusMessaging)(void *, void*);


typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t signal;
} Lock;

typedef struct __SynchZone {
    Lock Locked;

    unsigned long Id;

    void* pSharedData;

    List* plThreadInvolved;
    FreeFct* pFreeDataFunction;
} SynchZone;

typedef struct __Thread {
    unsigned long Id;
    char* Name;
    pthread_t IdThread;
    Lock Locked;

    SynchZone* pSynchZone;
    void* pThreadData;

    unsigned short Priority;
    boolean Silent;
    boolean AskLock;
    boolean AskSmartLock;
    boolean RunningState;
    boolean Paused;

    struct __TaskManager* pTaskManager;
    struct __BusManager* pBusManager;

    ThreadBeginning* pBeginningThread;
    ThreadRunning* pRunningThread;
    ThreadEnding* pEndingThread;

    ThreadRunCondition* pRunCondition;
    ThreadRunShared* pRunSharedZone;

    FreeFct* pFreeDataFunction;
} Thread;

typedef struct __TaskManager {
    unsigned long Id;

    List* plThread;
    List* plSynchZone;
} TaskManager;


typedef struct __BusManager {
    unsigned long Id;
    Lock Locked;

    int NbPool;

    BusMessaging* pGetMessage;
    BusMessaging* pPushMessage;

    ThreadBeginning* pBeginningThread;
    ThreadRunning* pRunningThread;
    ThreadEnding* pEndingThread;

    List* plThread;
    List* plMessage;
} BusManager;

typedef struct __Message {
    char* UId;
    time_t CreateDate;
    time_t ExecDate;
    unsigned short Priority;
    unsigned short Status

    void* pData;
} Message;

Thread* InitThread(void* , char*, unsigned long ,FreeFct* );

int YieldThread(Thread* );
int ResumeThread(Thread* );

SynchZone* InitSynchZone(Thread* , void*, FreeFct* );
int AddNewThreadInSynchZone(SynchZone* , Thread* ) ;
int AcquireSmartLock(SynchZone* );
int AcquireLock(SynchZone* );
int ReleaseLock(SynchZone* );

extern TaskManager* InitTaskManager(unsigned long );
extern int StartingTaskManager(TaskManager* );
extern int WaitingTaskManager(TaskManager* );

extern int AddNewThread(TaskManager* , Thread* );

extern int StartThread(TaskManager* , Thread *);

extern int AddNewSynchZone(TaskManager* , SynchZone* );

void FreeTaskManager(TaskManager* , boolean, boolean);
void FreeSynchZone(void* );
void FreeThread(void* );

void *GlobalThreadExecution(void*);

/*
    Default Thread Configuration **/
int DefaultBeginning(Thread*);
int DefaultRunning(Thread*);
int DefaultEnding(Thread*);
int DefaultRunCondition(Thread*);
int DefaultRunSharedZone(Thread*);

#endif
