#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "thread.h"
#include "b64.h"
#include "bl.h"

#ifndef __IOManage_h__
#define __IOManage_h__

#define SOCKET_STREAM_HANLDE   11
#define SOCKET_STATIC_HANLDE   12

#define FILE_STREAM_HANDLE     21
#define FILE_STATIC_HANDLE     22

#define MEMORY_STREAM_HANDLE   41
#define MEMORY_STATIC_HANDLE   42

#define SCREEN_STREAM_HANDLE   51
#define SCREEN_STATIC_HANDLE   52

//#define BLOCK_SIZE      65536
//#define BLOCK_SIZE      262144
//#define BLOCK_SIZE      1048576
#define BLOCK_SIZE    400000000


typedef int (IOTransform)(void *);

typedef struct __IOEntity {
    short Type;
    short Layer;
    boolean iStream;

    void* pHandle;

    unsigned long numSeq;
    unsigned long readSize;

    boolean redLight;

    unsigned long RecvLng;
    unsigned char * RecvStream;

    unsigned long SendLng;
    unsigned char * SendStream;

    IOTransform* pIOTransform;

    /** File */
    char*   FName;
    FILE*   pFile;

    Thread* pThread;
} IOEntity;


typedef struct __IOManage {

    unsigned short Nb;

    char* Name;
    char* Description;

    IOEntity* pIOMain;
    List*  plIOMultiplex;

    List* plThread;

    Thread* pThread;
} IOManage;


typedef struct __SData {
    unsigned long  numSeq;
    unsigned long  BuffLng;
    unsigned char* pBuffer;
} SData;


IOEntity* IOEntityScreen(IOTransform* , short , boolean );
int IOScreenManageRunning(Thread* );

IOEntity* IOEntityFile(char* , IOTransform* , short , boolean , char );
int IOFileBeginning(Thread* );
int IOFileManageRunning(Thread* );
int IOFileEnding(Thread* );

int IOSynckDispatch(Thread* );

int IOBase64(void*);
int IOCrypt(void*);
int IODCrypt(void*);

IOManage* CreateIOManager(IOEntity* , char* );
int IOStackManager(Thread* );
int IOManagerAddEntity(IOManage* , IOEntity* );
int LaunchIOManager(IOManage* );

void FreeThSafeIOEntity(void* );
void FreeThSafeIOManage(void* );
void FreeThSafeSData(void* );

#endif
