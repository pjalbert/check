#include "thread.h"

int main (int argc, const char * argv[]) {
    TaskManager* pTaskManager;
    SynchZone* pSZone;
    Thread* pThread;
    int* i;

    pTaskManager = InitTaskManager();

    srand(time(NULL));

    pThread = InitThread(NULL, "Stack-1.0", NULL);

    i = (int*)malloc(sizeof(int));
    *i = (int)0;
    pSZone = InitSynchZone(pThread, i, free);

    AddNewThread(pTaskManager, pThread);

    pThread = InitThread(NULL, "Stack-1.1", NULL);
    AddNewThreadInSynchZone(pSZone, pThread);
    AddNewThread(pTaskManager, pThread);

    pThread = InitThread(NULL, "Stack-1.2", NULL);
    AddNewThreadInSynchZone(pSZone, pThread);
    AddNewThread(pTaskManager, pThread);

    pThread = InitThread(NULL, "Stack-1.3", NULL);
    AddNewThreadInSynchZone(pSZone, pThread);
    AddNewThread(pTaskManager, pThread);

    AddNewSynchZone(pTaskManager, pSZone);
/**/

    pThread = InitThread(NULL, "Stack-2.0", NULL);

    i = (int*)malloc(sizeof(int));
    *i = (int)0;
    pSZone = InitSynchZone(pThread, i, free);

    AddNewThread(pTaskManager, pThread);

    pThread = InitThread(NULL, "Stack-2.1", NULL);
    AddNewThreadInSynchZone(pSZone, pThread);
    AddNewThread(pTaskManager, pThread);

    pThread = InitThread(NULL, "Stack-2.2", NULL);
    AddNewThreadInSynchZone(pSZone, pThread);
    AddNewThread(pTaskManager, pThread);

    pThread = InitThread(NULL, "Stack-2.3", NULL);
    AddNewThreadInSynchZone(pSZone, pThread);
    AddNewThread(pTaskManager, pThread);

    AddNewSynchZone(pTaskManager, pSZone);

    StartingTaskManager(pTaskManager);
    WaitingTaskManager(pTaskManager);

    return 0;
}
