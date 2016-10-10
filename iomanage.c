/*
 *  iomanage.c
 *  Idiome
 *
 *  Created by Pierre-Jean ALBERT on Mon Sep 08 2014.
 *  Copyright (c) 2014 __An Intellectual Projet__. All rights reserved.
 *
 */
#include "iomanage.h"


/**
    Concept working : An IOentity is an object that read and write stream from a handle (socket / memory / file)
        Goal is working about change - multiplexing - lock */

/**
    IOEntityScreen    */
IOEntity* IOEntityScreen(IOTransform* pTrans, short layer, boolean Stream) {
    IOEntity* pIOEntity;

    pIOEntity = (IOEntity*)malloc(sizeof(IOEntity));
    if (pIOEntity == NULL)
        return NULL;

    if (Stream)
        pIOEntity->Type = SCREEN_STREAM_HANDLE;
    else
        pIOEntity->Type = SCREEN_STATIC_HANDLE;

    pIOEntity->Layer = layer;

    pIOEntity->readSize = BLOCK_SIZE;
    pIOEntity->numSeq = 0;

    pIOEntity->RecvLng = 0;
    pIOEntity->RecvStream = NULL;
    pIOEntity->SendLng = 0;
    pIOEntity->SendStream = NULL;

    pIOEntity->redLight = false;

    if (pTrans)
        pIOEntity->pIOTransform = pTrans;
    else
        pIOEntity->pIOTransform = NULL;

    pIOEntity->pThread = (Thread*)InitThread(pIOEntity, "Printing Data on Screen", NULL);
    pIOEntity->pThread->pRunningThread = (ThreadRunning*)IOScreenManageRunning;
    pIOEntity->pThread->pRunSharedZone = (ThreadRunShared*)IOSynckDispatch;

    return pIOEntity;
}

int IOScreenManageRunning(Thread* pThread)
{
    IOEntity* pIOEntity;
    unsigned long iCnt;
    int rc;

    if (!pThread->Silent)
        printf("I am running thread %ld\n", (unsigned long)pThread->Id);

   pIOEntity = (IOEntity*)pThread->pThreadData;

    pThread->AskSmartLock = true;
    if (pIOEntity->redLight)
        return 1;


    if (pIOEntity->RecvLng == 0) {
        if (!pThread->Silent)
            printf("%ld - NoData \n", (unsigned long)pThread->Id);
        return 1;
    }

    if (pIOEntity->pIOTransform) {
        rc = pIOEntity->pIOTransform(pIOEntity);
        if (!rc) {
            pThread->RunningState = false;
            return 0;
        }
    } else {
        /**
            No transformation, we flip/flap Buffer just before synckzone */
        if (pIOEntity->SendLng == 0) {
            pIOEntity->SendStream = pIOEntity->RecvStream;
            pIOEntity->SendLng = pIOEntity->RecvLng;

            pIOEntity->RecvStream = NULL;
            pIOEntity->RecvLng = 0;
        }
    }

    if (pIOEntity->Layer > 0) {

        if (pIOEntity->SendLng > 0) {

            /** IOWrite */
            iCnt = fwrite(pIOEntity->SendStream, sizeof(unsigned char), pIOEntity->SendLng, stdout);

            if (iCnt != pIOEntity->SendLng) {
                printf("Trying to print %ld vs %ld\n", pIOEntity->SendLng, iCnt);
                pThread->RunningState = false;
                return 0;
            }

            free(pIOEntity->SendStream);
            pIOEntity->SendStream = NULL;
            pIOEntity->SendLng = 0;
        }
    }

    /**
        Question ? Have some work toDo ? */
    if (pIOEntity->SendLng == 0)
        pIOEntity->redLight = true;


    return 1;
}


/**
    Functions related to IOFileEntity */
IOEntity* IOEntityFile(char* FName, IOTransform* pTrans, short layer, boolean Stream, char c) {
    IOEntity* pIOEntity;
    FILE* pFile;
    SData* pSData;

    pIOEntity = (IOEntity*)malloc(sizeof(IOEntity));
    if (pIOEntity == NULL)
        return NULL;

    if (Stream)
        pIOEntity->Type = FILE_STREAM_HANDLE;
    else
        pIOEntity->Type = FILE_STATIC_HANDLE;

    pIOEntity->Layer = layer;

    pIOEntity->readSize = BLOCK_SIZE;
    pIOEntity->numSeq = 0;

    pIOEntity->FName = (char*) malloc(sizeof(char)*strlen(FName)+1);
    if (pIOEntity->FName) {
        memset(pIOEntity->FName, 0, sizeof(char)*strlen(FName)+1);
        memcpy(pIOEntity->FName, FName, sizeof(char)*strlen(FName));
    }

    if (c == 'r')
        pIOEntity->pFile = fopen (pIOEntity->FName, "rb");
    else
        pIOEntity->pFile = fopen (pIOEntity->FName, "wb");
    if (pIOEntity->pFile == NULL)
        return NULL;

    pIOEntity->RecvLng = 0;
    pIOEntity->RecvStream = NULL;
    pIOEntity->SendLng = 0;
    pIOEntity->SendStream = NULL;

    pIOEntity->pHandle = NULL;
    pIOEntity->redLight = false;

    if (pTrans)
        pIOEntity->pIOTransform = pTrans;
    else
        pIOEntity->pIOTransform = NULL;

    if (c == 'r') {
        pIOEntity->pThread = (Thread*)InitThread(pIOEntity, "Reading File Input", FreeThSafeIOEntity);
        pIOEntity->pThread->pBeginningThread = (ThreadRunning*)IOFileBeginning;
        pIOEntity->pThread->pRunningThread = (ThreadRunning*)IOFileManageRunning;
        pIOEntity->pThread->pEndingThread = (ThreadRunning*)IOFileEnding;
        pIOEntity->pThread->pRunSharedZone = (ThreadRunShared*)IOSynckDispatch;
    }
    else {
        pIOEntity->pThread = (Thread*)InitThread(pIOEntity, "Writing File Output", FreeThSafeIOEntity);
        pIOEntity->pThread->pRunningThread = (ThreadRunning*)IOFileManageRunning;
        pIOEntity->pThread->pEndingThread = (ThreadRunning*)IOFileEnding;
        pIOEntity->pThread->pRunSharedZone = (ThreadRunShared*)IOSynckDispatch;
    }

    if (layer == 0) {
        pSData = (SData*)malloc(sizeof(SData));
        if (pSData) {
            pSData->numSeq = pIOEntity->numSeq;
            pSData->BuffLng = 0;
            pSData->pBuffer = (unsigned char*)malloc(sizeof(unsigned char)*BLOCK_SIZE + 1);
            memset(pSData->pBuffer, 0, sizeof(unsigned char)*BLOCK_SIZE + 1);

            InitSynchZone(pIOEntity->pThread, pSData, FreeThSafeSData);
            setFreeFuntion((List**)&pIOEntity->pThread->pSynchZone->plThreadInvolved, FreeThread);
        }
    }


    return pIOEntity;
}

void FreeThSafeSData(void* pMemory)
{
    SData* pSData;
    List* pList;

    pSData = (SData*)pMemory;

    printf("FreeThSafeSData\n");

    if (pSData->pBuffer) {
        printf("pSData->pBuffer\n");
        free(pSData->pBuffer);
        pSData->pBuffer = NULL;
    }

    free (pSData);

}

void FreeThSafeIOEntity(void * pMemory)
{
    IOEntity* pIOEntity;

    pIOEntity = (IOEntity*)pMemory;

    printf("FreeIOEntity (Th-Name %s)\n", pIOEntity->pThread->Name);

    if(pIOEntity->FName) {
        free(pIOEntity->FName);
        pIOEntity->FName = NULL;
    }

    if(pIOEntity->RecvStream) {
        printf("FreeIOEntity -> RecvStream\n");
        free(pIOEntity->RecvStream);
        pIOEntity->RecvStream = NULL;
    }

    if(pIOEntity->SendStream) {
        printf("FreeIOEntity -> SendStream\n");
        free(pIOEntity->SendStream);
        pIOEntity->SendStream = NULL;
    }

    free(pIOEntity);

}

void FreeThSafeIOManage(void* pMemory)
{
    List* pList;
    IOManage* pIOManage;

    pIOManage = (IOManage*) pMemory;

    printf("FreeIOManage ...\n");

    if(pIOManage->Name) {
        free(pIOManage->Name);
        pIOManage->Name = NULL;
    }

    printf("FreeIOManage -> Description\n");

    if(pIOManage->Description) {
        free(pIOManage->Description);
        pIOManage->Description = NULL;
    }

    printf("FreeIOManage -> FreeIOEntity\n");
    pList = (List*)pIOManage->plIOMultiplex;
    CleanList(pList);

    pList = (List*)pIOManage->plThread;
    CleanList(pList);

    FreeThread(pIOManage->pThread);

    free(pIOManage);

}

int IOFileBeginning(Thread* pThread)
{
    IOEntity* pIOEntity;
    unsigned char* pBuffer;
    unsigned long iCnt;

    pThread->RunningState = true;

    if (!pThread->Silent)
        printf("Hi, i am just a starting thread %ld and now, let's go loading content file\n", (unsigned long)pThread->Id);

    pIOEntity = (IOEntity*)pThread->pThreadData;

    pBuffer = (char*)malloc(sizeof(unsigned char)*pIOEntity->readSize + 1);
    if (!pBuffer)
        return 0;
    memset(pBuffer, 0, sizeof(unsigned char)*pIOEntity->readSize + 1);

    iCnt = fread(pBuffer, sizeof(unsigned char), pIOEntity->readSize, pIOEntity->pFile);

    pIOEntity->RecvStream = (unsigned char*) pBuffer;
    pIOEntity->RecvLng = iCnt;
    pIOEntity->numSeq++;

    if (!pThread->Silent)
        printf("Ending IOFileBeginning % ld\n", (unsigned long)pThread->Id);

    return 1;
}

int IOFileEnding(Thread* pThread)
{
    IOEntity* pIOEntity;

    if (!pThread->Silent)
        printf("ok, i am an ending thread %ld\n", (unsigned long)pThread->Id);

    pIOEntity = (IOEntity*)pThread->pThreadData;

    fclose(pIOEntity->pFile);

    return 1;
}

int IOFileManageRunning(Thread* pThread)
{
    IOEntity* pIOEntity;
    unsigned long iCnt;
    unsigned char *pBuffer;
    int rc;

    if (!pThread->Silent)
        printf("Well, i am running thread %ld/%ld\n", (unsigned long)pThread->Id, pThread->RunningState);


    pIOEntity = (IOEntity*)pThread->pThreadData;

    if (pIOEntity->redLight) {
        if (!pThread->Silent)
            printf("No Task %ld\n", (unsigned long)pThread->Id);
        return 1;
    }


    pThread->AskSmartLock = true;

    if (pIOEntity->Layer == 0) {

        if (pIOEntity->RecvLng == 0) {
            pBuffer = (char*)malloc(sizeof(unsigned char)*pIOEntity->readSize + 1);
            if (!pBuffer) {
                pThread->RunningState = false;
                return 0;
            }

            /** IORead */
            iCnt = fread(pBuffer, sizeof(unsigned char), pIOEntity->readSize, pIOEntity->pFile);
            if (iCnt > 0) {
                pIOEntity->RecvStream = (unsigned char*) pBuffer;
                pIOEntity->RecvLng = iCnt;
                pIOEntity->numSeq++;
            }
            else {
                if (!pThread->Silent)
                        printf("No more data\n");
                pThread->RunningState = false;
                printf("Forgotten free pBuffer [Fix]\n");
                free(pBuffer);
            }
        }
    }
    else {
        if (pIOEntity->RecvLng == 0) {
            if (!pThread->Silent)
                printf("%ld - NoData \n", (unsigned long)pThread->Id);
            return 1;
        }
    }


    if (pIOEntity->pIOTransform) {

        rc = pIOEntity->pIOTransform(pIOEntity);
        if (!rc) {
            pThread->RunningState = false;
            return 0;
        }
    } else {
        /**
            No transformation, we flip/flap Buffer just before synckzone */
        if (pIOEntity->SendLng == 0) {
            pIOEntity->SendStream = pIOEntity->RecvStream;
            pIOEntity->SendLng = pIOEntity->RecvLng;

            pIOEntity->RecvStream = NULL;
            pIOEntity->RecvLng = 0;

            if (!pThread->Silent)
                printf("%ld - pIOEntity->(%ld/%ld)\n", (unsigned long)pThread->Id, pIOEntity->RecvLng, pIOEntity->SendLng);
        }
    }

    if (pIOEntity->Layer > 0) {

        if (pIOEntity->SendLng > 0) {
            /** IOWrite */
            iCnt = fwrite(pIOEntity->SendStream, sizeof(unsigned char), pIOEntity->SendLng, pIOEntity->pFile);
            if (!pThread->Silent)
                printf("%ld - pIOEntity->(%ld/%ld) %ld\n", (unsigned long)pThread->Id, pIOEntity->RecvLng, pIOEntity->SendLng, iCnt);

            if (iCnt != pIOEntity->SendLng) {
                if (!pThread->Silent)
                        printf("Trying to write %ld vs %ld\n", pIOEntity->SendLng, iCnt);
                pThread->RunningState = false;
                return 0;
            }
            else
                fflush(pIOEntity->pFile);

            free(pIOEntity->SendStream);
            pIOEntity->SendStream = NULL;
            pIOEntity->SendLng = 0;
        }
    }

    /**
        Question ? Have some work toDo ? */
    if (pIOEntity->pThread->RunningState) {
        if (pIOEntity->Layer == 0) {
            if ((pIOEntity->RecvLng == 0) && (pIOEntity->SendLng > 0)) {
                pIOEntity->redLight = true;
                if (!pThread->Silent)
                    printf("%ld - redLight is on\n", (unsigned long)pThread->Id);
            }
        }
        else {
            if ((pIOEntity->SendLng > 0) && (pIOEntity->RecvLng == 0))
                pIOEntity->redLight = true;
        }
    }

    return 1;
}

int IOBase64(void* pTmp)
{
    IOEntity* pIOEntity;

    pIOEntity = (IOEntity *)pTmp;

    if (!pIOEntity->pThread->Silent)
        printf ("### IOBase64 %ld - %ld (%ld) ###\n", pIOEntity->RecvLng, pIOEntity->SendLng, Base64encode_len(pIOEntity->RecvLng));

    pIOEntity->SendStream = (unsigned char*)B64Encode((unsigned char*)pIOEntity->RecvStream, pIOEntity->RecvLng);
    if (!pIOEntity->SendStream) {
        if (!pIOEntity->pThread->Silent)
            printf("B64 Error allocating\n");
        return 0;
    }
    pIOEntity->SendLng = strlen(pIOEntity->SendStream);
    if (!pIOEntity->pThread->Silent)
        printf("B64 Encoding len %ld\n", pIOEntity->SendLng);

    free(pIOEntity->RecvStream);
    pIOEntity->RecvStream = NULL;
    pIOEntity->RecvLng = 0;

    return 1;
}

int IOCrypt(void* pTmp)
{
    IOEntity* pIOEntity;
    char * iSecret = "L'avenir a le don d'arriver sans prévenir";

    pIOEntity = (IOEntity *)pTmp;

    if (!pIOEntity->pThread->Silent)
        printf ("### IOCrypt %ld - %ld ###\n", pIOEntity->RecvLng, pIOEntity->SendLng);

    pIOEntity->SendStream = (unsigned char *)BFishCrypt(iSecret, pIOEntity->RecvStream, pIOEntity->RecvLng);
    if (!pIOEntity->SendStream) {
        if (!pIOEntity->pThread->Silent)
            printf("BCrypt Error allocating\n");
        return 0;
    }

    if (pIOEntity->RecvLng % 8 == 0)
        pIOEntity->SendLng = pIOEntity->RecvLng + 8;
    else
        pIOEntity->SendLng = (unsigned long)(8+8*((int)(pIOEntity->RecvLng/8)));;

    free(pIOEntity->RecvStream);
    pIOEntity->RecvStream = NULL;
    pIOEntity->RecvLng = 0;

	return 1;
}


int IODCrypt(void* pTmp)
{
    IOEntity* pIOEntity;
    char * iSecret = "L'avenir a le don d'arriver sans prévenir";
	unsigned char* pContent;

    pIOEntity = (IOEntity *)pTmp;

    if (!pIOEntity->pThread->Silent)
        printf ("### IOCrypt %ld - %ld ###\n", pIOEntity->RecvLng, pIOEntity->SendLng);

    pIOEntity->SendLng = BFishUncrypt(iSecret, pIOEntity->RecvStream, pIOEntity->RecvLng);
    pIOEntity->SendStream = pIOEntity->RecvStream;

    if (!pIOEntity->pThread->Silent)
        printf ("### %ld - %ld ###\n", pIOEntity->RecvLng, pIOEntity->SendLng);

    pIOEntity->RecvStream = NULL;
    pIOEntity->RecvLng = 0;

	return 1;
}


int IOSynckDispatch(Thread* pThread)
{
    SynchZone* pSZone;
    IOEntity* pIOEntity;
    SData* pSData;

    /**
        Multicast */
    pSZone = (SynchZone*)pThread->pSynchZone;
    pSData = (SData*)pSZone->pSharedData;
    pIOEntity = (IOEntity*)pThread->pThreadData;

    if (!pThread->Silent)
        printf("SZone for thread %ld\n", (unsigned long)pThread->Id);


    if (pIOEntity->Layer == 0) {
        if (pIOEntity->SendLng > 0) {
            if (!pThread->Silent)
                printf("pSZone -> Len %ld\n", pIOEntity->SendLng);
            memcpy(pSData->pBuffer, pIOEntity->SendStream, pIOEntity->SendLng);
            pSData->BuffLng = pIOEntity->SendLng;
            pSData->numSeq = pIOEntity->numSeq;

            free(pIOEntity->SendStream);
            pIOEntity->SendStream = NULL;
            pIOEntity->SendLng = 0;
        }
    } else {
        if ((pIOEntity->RecvLng == 0) && (pIOEntity->numSeq < pSData->numSeq)) {
            if (pSData->BuffLng > 0) {
                pIOEntity->RecvStream = (unsigned char*) malloc(BLOCK_SIZE*sizeof(unsigned char));
                memset(pIOEntity->RecvStream, 0, BLOCK_SIZE*sizeof(unsigned char));
                memcpy(pIOEntity->RecvStream, pSData->pBuffer, pSData->BuffLng);
                pIOEntity->RecvLng = pSData->BuffLng;
                if (!pThread->Silent)
                    printf("pSZone <- Len %ld \n", pIOEntity->RecvLng);
                pIOEntity->numSeq++;
            }
        }
    }

    return 1;
}

/**
    */
IOManage* CreateIOManager(IOEntity* pIOEntity, char* pName) {
    IOManage* pIOManage;

    pIOManage = (IOManage*)malloc(sizeof(IOManage));
    if (!pIOManage)
        return NULL;

    if (pName) {
        pIOManage->Name = (char *)malloc(sizeof(char)*strlen(pName) + 1);
        memset(pIOManage->Name, 0, sizeof(char)*strlen(pName) + 1);
        memcpy(pIOManage->Name, pName, strlen(pName));
    }
    else
        pIOManage->Name = NULL;

    pIOManage->Nb = 0;
    pIOManage->Description = NULL;

    pIOManage->pIOMain = pIOEntity;
    pIOManage->plIOMultiplex = NULL;
    pIOManage->plThread = NULL;

    pIOManage->pThread = (Thread*)InitThread(pIOManage, "Multiplex manage", NULL);
    pIOManage->pThread->pRunningThread = (ThreadRunning*)IOStackManager;

    return pIOManage;
}

int IOStackManager(Thread* pThread)
{
    IOManage* pIOManage;
    IOEntity* pIOEntity;
    SynchZone* pSZone;
    SData* pSData;
    Thread* pThreadWorker;
    int i;
    List* pList;
    Chaine* pChaine;

    if (!pThread->Silent)
        printf("Well, i am running thread %ld/%ld\n", (unsigned long)pThread->Id, pThread->RunningState);

    pIOManage = (IOManage*)pThread->pThreadData;

    pList = (List*)pIOManage->plThread;
    pChaine = pList->Main;
    i = 0;
    while (pChaine) {
        pThreadWorker = (Thread*)pChaine->Member;

        pIOEntity = (IOEntity*)pThreadWorker->pThreadData;
        if (0 == 1) {
            printf("Thread %ld IOEntity state\n", pThreadWorker->Id);
            printf("\tpIOEntity->redLight\t%d\n", pIOEntity->redLight);
            printf("\tpIOEntity->RecvLng\t%d\n", pIOEntity->RecvLng);
            printf("\tpThreadWorker->RunningState\t%d\n",  pThreadWorker->RunningState);
        }

        if ((!pIOEntity->redLight) && (pIOEntity->numSeq == pIOManage->pIOMain->numSeq) && (pIOEntity->numSeq > 0) && (pIOEntity->RecvLng == 0))
            i++;

        pChaine = pChaine->Next;
    }

    if (i == pIOManage->Nb) {

        pIOEntity = (IOEntity*)pIOManage->pIOMain;
        if (pIOManage->pIOMain->pThread->RunningState == false) {
            pList = (List*)pIOManage->plThread;
            pChaine = pList->Main;
            while (pChaine) {
                pThreadWorker = (Thread*)pChaine->Member;

                pThreadWorker->RunningState = false;
                pChaine = pChaine->Next;
            }
            pThread->RunningState = false;
        }
        else {
            pSZone = (SynchZone*)pIOEntity->pThread->pSynchZone;
            pSData = (SData*)pSZone->pSharedData;

            pSData->numSeq = pIOEntity->numSeq;
            pSData->BuffLng = 0;
            memset(pSData->pBuffer, 0, sizeof(unsigned char)*BLOCK_SIZE + 1);

            pIOEntity->redLight = false;
        }
    }

    return 1;
}


int IOManagerAddEntity(IOManage* pIOManage, IOEntity* pIOEntity) {
    IOEntity* pMainIOEntity;
    unsigned char* pBuffer;

    pMainIOEntity = (IOEntity*)pIOManage->pIOMain;
    pIOManage->Nb++;

    AddNewThreadInSynchZone((SynchZone*) pMainIOEntity->pThread->pSynchZone, pIOEntity->pThread);

    AddMemberOnBottom(&pIOManage->plIOMultiplex, pIOEntity);
    setFreeFuntion(&pIOManage->plIOMultiplex, FreeThSafeIOEntity);

    AddMemberOnBottom(&pIOManage->plThread, pIOEntity->pThread);
    setFreeFuntion(&pIOManage->plIOMultiplex, FreeThread);

    return 1;
}

int LaunchIOManager(IOManage* pIOManage)
{
    TaskManager* pTaskManager;
    List* pList;
    Chaine* pChaine;
    Thread* pThread;
    SynchZone* pSynchZone;

    pTaskManager = InitTaskManager();

    AddNewThread(pTaskManager, pIOManage->pIOMain->pThread);
    AddNewThread(pTaskManager, pIOManage->pThread);

    pSynchZone = (SynchZone*)pIOManage->pIOMain->pThread->pSynchZone;
    if (pSynchZone) {
        AddMemberOnTop((List**)&pTaskManager->plSynchZone, pSynchZone);
        setFreeFuntion((List**)&pTaskManager->plSynchZone, FreeSynchZone);
    }

    pList = (List*)pIOManage->plThread;
    if (!pList) {
        printf("No Thread task define\n");
        return 0;
    }
    else
        setFreeFuntion(&pList, FreeThread);

    pChaine = pList->Main;
    while (pChaine) {
        pThread = (Thread*)pChaine->Member;
        printf("Adding... %s\n", pThread->Name);

        AddNewThread(pTaskManager, pThread);

        pChaine = pChaine->Next;
    }

    StartingTaskManager(pTaskManager);
    WaitingTaskManager(pTaskManager);
        printf("pTaskManager\n");

    /** ToDo -- Set Free Function */
    pList = (List*)pTaskManager->plThread;
    if (pList)
        setFreeFuntion(&pList, FreeThread);
    else
        printf("pTaskManager->plThread is null\n");

    pList = (List*)pTaskManager->plSynchZone;
    if (!pList)
        printf("pTaskManager->plSynchZone is null\n");


    printf("Begin Free TManager Struct...\n");
    printf("pIOManage->pIOMain->pThread %s\n", pIOManage->pIOMain->pThread->Name);
    printf("pIOManage->pThread %s\n", pIOManage->pThread->Name);

    FreeTaskManager(pTaskManager, true, true);

    return 1;
}
