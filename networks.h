#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <sys/utsname.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <ifaddrs.h>
#include <netdb.h>


#include <net/if.h>
#include <sys/ioctl.h>

#include "uuid.h"
#include "list.h"
#include "thread.h"

#ifndef __AIP_Networks_h__
#define __AIP_Networks_h__

typedef enum {IPV4 = 1, IPV6 = 2} IPFamily;
typedef enum {TCP = 4 , UDP = 8} Protocol;
typedef enum {CLIENT = 16 , PROXY = 32, SERVER = 64} ServRole;

#define DEFAULT_STACK   10
#define DEFAULT_PACKET  2048
#define DEFAULT_TIMER   60

typedef int (DispatchEvent)(void *, unsigned long , unsigned long);

typedef int (ServerLoop)(void * );
typedef int (OnConnect)(void * );
typedef int (OnRecv)(void * );
typedef int (OnSend)(void * );
typedef int (OnDisconnect)(void * );


typedef struct __ProtocolHandler {
    char* SName;
    char* LongDesc;

    uint packetSize;
    uint TimeOut;

    OnConnect*  pOnConnect;
    OnDisconnect* pOnDisconnect;

    ServerLoop* pSLoop;

    OnRecv*     pOnRecv;
    OnSend*     pOnSend;

    void*       pData;

} ProtocolHandler;


typedef struct __ifCfg {
    IPFamily Type;
    char* EthName;

    char* iNet;
    char* iMsk;
    char* bCast;

    uchar* HWaddr;

    int   MTU;
    short Flags;
} ifCfg;


typedef struct __Signature {
    char* HName;
    char* OSDesc;
    char* UId;

	struct ifaddrs *ifAddress;

    List* plifCfg;
} Signature;


typedef struct __Networks {
    boolean isRunning;

    fd_set Master_Desc;
    uint   SockMax_Desc;

    uint AllSockets[Max1024];
    ushort nbSockets;

    uint AllListening[Max1024];
    ushort nbListening;

    uint AllClients[Max1024];
    ushort nbClients;

    List* plConnect;
    List* plListen;

    Uuid* pUuid; /* Entropy Engine */

    Signature* pSign;

    Thread* pThread;
    List*   plThread;
    TaskManager* pTaskManager;
} Networks;


typedef struct __Communication {
    IPFamily IPType;
    Protocol PPType;
    ServRole CommCh;

    uint Socket;
    boolean isClosed;

    uchar* Init;
    ulong  Initlen;

    uchar* Recv;
    ulong  Recvlen;

    uchar* Send;
    ulong  Sendlen;

    Networks* pNetworks;

    struct sockaddr* IP_Addr;

    struct __Connect* pConnect;
    struct __Listener* pListener;

    ProtocolHandler* pPHandler;

	time_t CTime;

    Thread* pThread;
} Communication;


typedef struct __Connect {
    IPFamily IPType;
    Protocol PPType;

    char* HostName;
    struct sockaddr* IP_Addr;
    unsigned long Port;

    uint clientSocket;
    boolean isConnect;

    ProtocolHandler* pPHandler;

    Communication* pComm;

    Networks* pNetworks;
    Thread* pThread;
} Connect;

typedef struct __Listener {
    IPFamily IPType;
    Protocol PPType;

    char* EthName;
    unsigned long Port;

    uint   serverSocket;
    ushort Stack;

    uint   connectSocket[Max256];
    ushort nbSockets;

    ProtocolHandler* pPHandler;

    Networks* pNetworks;
    List* plCommunication;
    List* plThread;
} Listener;

List* GetNetworksList(struct ifaddrs *);
Signature* InitSignature(char* , Uuid*);
Networks* InitNetwoks(char* );
Listener* CreateListener(Networks* , char*, unsigned long, IPFamily,  Protocol);
char* GetIPFromEthName(List* , char*, IPFamily);

Communication* CreateCommunicationServer(Networks* , Listener* , uint , struct sockaddr* );
Communication* CreateCommunicationClient(Networks* , Connect* );

Communication* GetCommunicationFromSockaddr(List* ,IPFamily, struct sockaddr * );
int CleanCommunicationTimer(Listener* pListener);
int StartCommunication(Thread* pThread);
int StopCommunication(Thread* pThread);
int LoopCommunication(Thread* pThread);
void FreeCommunicationThread(void *);

// Echo Server Implementation
int OnConnect_EchoProtocol(void* );
int ServerLoop_EchoProtocol(void* );
int OnDisconnect_EchoProtocol(void* );

// UUID Server Implementation
int OnConnect_UuidProtocol(void* );
int ServerLoop_UuidProtocol(void* );
int OnDisconnect_UuidProtocol(void* );

// Simple HTTP Crawler Implementation
int OnConnect_HTTPLoad(void* );
int ServerLoop_HTTPLoad(void* );
int OnDisconnect_HTTPLoad(void* );


int NetworksBeginning(Thread* pThread);
int NetworksDispatch(Thread* pThread);
int NetworksEnding(Thread* pThread);

#endif
