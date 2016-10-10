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


#include "list.h"
#include "thread.h"

#ifndef __AIP_Server_h__
#define __AIP_Server_h__

typedef int (OnConnect)(void *, unsigned long );
typedef int (DispatchEvent)(void *, unsigned long , unsigned long);
typedef int (ServerLoop)(void *);
typedef int (OnDisconnect)(void *, unsigned long );

typedef struct __ProtocolHandler {

	char* Name;
	boolean Multicast;

	OnConnect* pOnConnect;

	List* plRecvMessages;
	List* plSendMessages;

	DispatchEvent* pRunning;

	OnDisconnect* pOnDisconnect;

} ProtocolHandler;


typedef struct __ArtiComm {
	fd_set master_fds;

	/** Socket stuff */
	List* plGlobalServer;
	List* plConnectClient;

	/** IP Desc */
	struct ifaddrs *ifAddress;

	/** AIP */
	char* AIPSName;
	char* OSDesc;
	char* Signature;

  /** Protocol */
	List* plProtocolHandler;

	/** List Command
	CentralExec* pCentralExec;*/

	TaskManager* pTaskManager;
} ArtiComm;


typedef struct __GlobalServer {
	char  IPVersion[5];
	unsigned long Port;

	struct addrinfo *pServerInfos;
	struct sockaddr_in6* iPv6;
	struct sockaddr_in* iPv4;

	unsigned long sockDesc;

	unsigned char * RecvByte;
	unsigned char * SendByte;

	ProtocolHandler* pPHandler;
	ServerLoop* pLooping;

	Thread* pThread;
	ArtiComm* pArtiComm;
} GlobalServer;

typedef struct __ConnectClient {
	char  IPVersion[5];
	unsigned long Port;

	struct addrinfo *pServerInfos;
	struct sockaddr_in6* iPv6;
	struct sockaddr_in* iPv4;

	unsigned long sockDesc;

	unsigned char * RecvByte;
	unsigned char * SendByte;

	ProtocolHandler* pPHandler;

	Thread* pThread;
	ArtiComm* pArtiComm;
} ConnectClient;


/**	GlobalServer* pGlobalServer */
int DefaultOnConnect(GlobalServer* , unsigned long );
int DefaultOnDisconnect(GlobalServer* , unsigned long );

GlobalServer* InitGlobalServer(ArtiComm* , char* , char* , char*);
int DefaultDispatchServer(GlobalServer* );
void FreeGlobalServer(void* );

int EchoServer(GlobalServer*, unsigned long , unsigned long ) ;
// int ProxyServer(GlobalServer*, unsigned long , unsigned long ) ;


/** 	Thread* pThread */
int DefaultServerRunning(Thread* );



#endif
