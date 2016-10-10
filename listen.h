#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "thread.h"

#ifndef __Listen_h__
#define __Listen_h__


typedef int (TreatMsg)(void *);

typedef struct __Message {
    int Type;

    unsigned long MetaLen;
    char Key[32];
    unsigned long Len;
    unsigned char* Content; 
    char Signature[32];

    boolean isComplete;

    (TreatMsg*) pOnSendingTreatment;
    (TreatMsg*) pOnRecvingTreatment;
} Message;


typedef struct __Protocol {

    List* pMessage;
} Protocol;



typedef struct __Listener {
    socket 
    port

    Protocol* pProtocol:
    
} Listener;


typedef struct __Client {

    Protocol* pProtocol:
} Server;



typedef struct __Server {

    struct * __Listener;
    List* plCient;

//    typedef __Crypto * pCrypto;
    
    TaskManager* pTaskManager;
} Server;

#endif 
