/*
 *  server.c
 *  Idiome
 *
 *  Created by Pierre-Jean ALBERT on Wed May 28 2014.
 *  Copyright (c) 2014 __An Intellectual Projet__. All rights reserved.
 *
 */
#include "server.h"

#define AIP_VIEW_DEBUG

ProtocolHandler* InitProtocolHandler(char * Name) {
	ProtocolHandler* pPHandler;

	pPHandler = (ProtocolHandler* )malloc (sizeof(ProtocolHandler));
	if (pPHandler == NULL)
		return NULL;

	if (Name) {
		pPHandler->Name = (char*)malloc(strlen(Name)*sizeof(char) + 1);
		memset(pPHandler->Name, 0, strlen(Name)*sizeof(char) + 1);
		strcpy(pPHandler->Name, Name);
	}
	else {
		pPHandler->Name = (char*)malloc(25*sizeof(char) + 1);
		memset(pPHandler->Name, 0, 25*sizeof(char) + 1);
		strcpy(pPHandler->Name, "Default Multi-server Echo");
	}

	pPHandler->pOnConnect = (OnConnect *)DefaultOnConnect;
	pPHandler->pRunning = (DispatchEvent*)EchoServer;
	pPHandler->pOnDisconnect = (OnDisconnect*)DefaultOnDisconnect;

/*	pPHandler->pRecvMessages = NULL;
	pPHandler->pSendMessages = NULL; */

	return pPHandler;
}

//int AddNewMessage(int Type, char* Name, in )




int FreeProtocolHandler(ProtocolHandler* pPHandler) {

	if (pPHandler->Name)
		free(pPHandler->Name);

	free(pPHandler);

	return 1;
}

int AddListeningPort(GlobalServer *pGlobalServer, char* IPVersion, char* Host, char* Port) {
	struct addrinfo Hints, *pTmp;
	int OptVal, rc;

	memset (&Hints, 0, sizeof (Hints));
    Hints.ai_socktype = SOCK_STREAM;

	if (memcmp(IPVersion, "IPv4", 4) == 0) Hints.ai_family = AF_INET;
 	if (memcmp(IPVersion, "IPv6", 4) == 0) Hints.ai_family = AF_INET6;
 	if (memcmp(IPVersion, "Both", 4) == 0) Hints.ai_family = AF_UNSPEC;

 	Hints.ai_flags = AI_PASSIVE;

	rc = getaddrinfo(Host, Port, &Hints, &(pGlobalServer->pServerInfos));
	if (rc != 0)
		return 0;

	for (pTmp = pGlobalServer->pServerInfos; pTmp != NULL; pTmp = pTmp->ai_next) {
		if(pTmp->ai_family == AF_INET6) {
            if (!pGlobalServer->iPv6) {
                pGlobalServer->iPv6 = (struct sockaddr_in6 *)malloc(sizeof(struct sockaddr_in6) + 1);
                memset(pGlobalServer->iPv6, 0, sizeof(struct sockaddr_in6) + 1);
                memcpy (pGlobalServer->iPv6, pTmp->ai_addr, pTmp->ai_addrlen);
            }
            break;
        }

        if(pTmp->ai_family == AF_INET) {
            if (!pGlobalServer->iPv4) {
                pGlobalServer->iPv4 = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in) + 1);
                memset(pGlobalServer->iPv4, 0, sizeof(struct sockaddr_in) + 1);
                memcpy (pGlobalServer->iPv4, pTmp->ai_addr, pTmp->ai_addrlen);
            }
            break;
        }
	}

	OptVal = 1;
 	if (memcmp(pGlobalServer->IPVersion, "IPv4", 4) == 0) {
		pGlobalServer->sockDesc = socket(AF_INET, SOCK_STREAM, 0);
		if (pGlobalServer->sockDesc < 0)
			return 0;

		setsockopt(pGlobalServer->sockDesc, SOL_SOCKET, SO_REUSEADDR, &OptVal, sizeof(OptVal));

#ifdef AIP_VIEW_DEBUG
		printf("Print struct sockaddr_in ...\n");
		printf("\t ..sin_family : %d\n", pGlobalServer->iPv4->sin_family);
		printf("\t ..sin_addr : %s\n", pGlobalServer->iPv4->sin_addr);
		printf("\t ..sin_port : %ld\n", htons(pGlobalServer->iPv4->sin_port));
#endif
	    rc = bind(pGlobalServer->sockDesc, (struct sockaddr *)((struct sockaddr_in *)pGlobalServer->iPv4), sizeof(struct sockaddr_in));
		if (rc < 0) {
			printf("Error Binding Socket : %d\n", rc);
			return 0;
		}
 	}
 	else {
		pGlobalServer->sockDesc = socket(AF_INET6, SOCK_STREAM, 0);
		if (pGlobalServer->sockDesc < 0)
			return 0;

		setsockopt(pGlobalServer->sockDesc, SOL_SOCKET, SO_REUSEADDR, &OptVal, sizeof(OptVal));

#ifdef AIP_VIEW_DEBUG
		printf("Print struct sockaddr_in6 ...\n");
		printf("\t ..sin6_family : %d\n", pGlobalServer->iPv6->sin6_family);
		printf("\t ..sin6_addr : %s\n", pGlobalServer->iPv6->sin6_addr);
		printf("\t ..sin6_port : %ld\n", htons(pGlobalServer->iPv6->sin6_port));
#endif

        rc = bind(pGlobalServer->sockDesc, (struct sockaddr *)((struct sockaddr_in6 *)pGlobalServer->iPv6), sizeof(struct sockaddr_in6));
        if (rc < 0) {
			printf("Error Binding Socket : %d\n", rc);
			return 0;
		}
 	}

 	rc = listen(pGlobalServer->sockDesc, 10);
	if (rc < 0) {
		printf("Error Listening Socket : %d\n", rc);
		return 0;
	}

#ifdef AIP_VIEW_DEBUG
		printf("Success listening on port %s...\n", Port);
#endif

	return 1;
}


GlobalServer* InitGlobalServer(ArtiComm* pArtiComm, char* IPVersion, char* Host, char* Port) {
	GlobalServer* pGlobalServer;
	int rc;

	pGlobalServer = (GlobalServer*)malloc(sizeof(GlobalServer));
	if (pGlobalServer == NULL)
		return NULL;

	if ((memcmp(IPVersion, "IPv4", 4) != 0)
		&& (memcmp(IPVersion, "IPv6", 4) != 0)
		&& (memcmp(IPVersion, "Both", 4) != 0))
		return NULL;

	pGlobalServer->pArtiComm = pArtiComm;

	memset(pGlobalServer->IPVersion, 0, 5);
	memcpy(pGlobalServer->IPVersion, IPVersion, 4);
	pGlobalServer->Port = atol(Port);

	pGlobalServer->iPv4 = NULL;
	pGlobalServer->iPv6 = NULL;

	rc = AddListeningPort(pGlobalServer, pGlobalServer->IPVersion, Host, Port);
	if (!rc)
		return NULL;;

	pGlobalServer->pPHandler = (ProtocolHandler *)InitProtocolHandler("Multi-Server.Echo");
    pGlobalServer->pLooping = (ServerLoop*)DefaultDispatchServer;
	pGlobalServer->pThread = InitThread(pGlobalServer, "Server.Loop", FreeGlobalServer);
	pGlobalServer->pThread->pRunningThread = (ThreadRunning *)DefaultServerRunning;
    pGlobalServer->pThread->pFreeDataFunction = NULL;

	pGlobalServer->pThread->Id = atol(Port);

	AddNewThread(pGlobalServer->pArtiComm->pTaskManager, pGlobalServer->pThread);

#ifdef AIP_VIEW_DEBUG
		printf("Exiting InitGlobalServer\n");
#endif
	return pGlobalServer;
}

void FreeGlobalServer(void* pMemory) {
    GlobalServer* pGlobalServer;

    pGlobalServer = (GlobalServer*)pMemory;

	if (pGlobalServer->pPHandler)
		FreeProtocolHandler(pGlobalServer->pPHandler);

	free(pGlobalServer);

}

int DefaultServerRunning(Thread* pThread) {
	GlobalServer* pGlobalServer;
	int rc;

	pGlobalServer = (GlobalServer *)pThread->pThreadData;
	rc = pGlobalServer->pLooping(pGlobalServer);

	pThread->RunningState = 0;

	return 1;
}

ArtiComm* InitArtiComm(char* ServerName, char* Signature) {
	ArtiComm* pArtiComm;
	int rc, Lng;
	struct utsname my_uname;

	pArtiComm = (ArtiComm*) malloc (sizeof(ArtiComm));
	if (pArtiComm == NULL)
		return NULL;

	if (ServerName) {
		pArtiComm->AIPSName = (char*)malloc(strlen(ServerName)*sizeof(char) + 1);
		memset(pArtiComm->AIPSName, 0, strlen(ServerName)*sizeof(char) + 1);
		strcpy(pArtiComm->AIPSName, ServerName);
	}

	if (Signature) {
		pArtiComm->Signature = (char*)malloc(strlen(Signature)*sizeof(char) + 1);
		memset(pArtiComm->Signature, 0, strlen(Signature)*sizeof(char) + 1);
		strcpy(pArtiComm->Signature, Signature);
	}
	else {
		/** SHA-512 "Qui... le monde..." */
		pArtiComm->Signature = (char*)malloc(128*sizeof(char) + 1);
		memset(pArtiComm->Signature, 0, 128*sizeof(char) + 1);
		strcpy(pArtiComm->Signature, "03fab7d9cda9103a9c65ef891734cf34f9e43051e249c58d581f2128e7914efcdcdeac2bf722049c47a326f7b1eac230e83a5bea6a31482cc2b469ba924c5aa9");
	}

	uname(&my_uname);
	Lng = strlen(my_uname.sysname) + strlen(my_uname.nodename) + strlen(my_uname.release)
		+ strlen(my_uname.version) + strlen(my_uname.machine) + 5;
	pArtiComm->OSDesc = (char*)malloc(Lng*sizeof(char) + 1);
  sprintf(pArtiComm->OSDesc, "%s %s %s %s %s", my_uname.sysname, my_uname.nodename, my_uname.release,my_uname.version,my_uname.machine);

  /** FreeAIP -> freeifaddrs(pArtiComm->ifAddress); */
	rc = getifaddrs(&(pArtiComm->ifAddress));
	if (rc < 0)
		return NULL;

#ifdef AIP_VIEW_DEBUG
	ViewifAddress(pArtiComm->ifAddress);
#endif

	pArtiComm->pTaskManager = InitTaskManager();
	printf("%s\nTaskManager Id %lu\n", pArtiComm->OSDesc, pArtiComm->pTaskManager->Id);

	pArtiComm->plGlobalServer = NULL;
	pArtiComm->plConnectClient = NULL;
	pArtiComm->plProtocolHandler = NULL;

	return pArtiComm;
}


void FreeArtiComm(ArtiComm* pArtiComm) {

	if (pArtiComm->AIPSName) {
		free(pArtiComm->AIPSName);
		pArtiComm->AIPSName = NULL;
	}

	if (pArtiComm->Signature) {
		free(pArtiComm->Signature);
		pArtiComm->Signature = NULL;
	}

	free(pArtiComm->OSDesc);
	pArtiComm->OSDesc = NULL;

  freeifaddrs(pArtiComm->ifAddress);
  pArtiComm->ifAddress = NULL;

	FreeTaskManager(pArtiComm->pTaskManager, true, true);
	pArtiComm->pTaskManager = NULL;

	free(pArtiComm);
}


int ViewifAddress(	struct ifaddrs *ifAddress) {
	struct ifaddrs *ifa;
  int rc;
  char HostName[512];
  char IPName[128];

	for (ifa = ifAddress; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
    	continue;

		if (ifa->ifa_addr->sa_family == AF_INET) {
			rc = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), HostName, sizeof (HostName), NULL, 0, 0);
			rc = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), IPName, sizeof (IPName), NULL, 0, 1);
			printf("iPv4 : %s -> %s %s\n", ifa->ifa_name, IPName, HostName);
		}

		if (ifa->ifa_addr->sa_family == AF_INET6) {
			rc = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in6), HostName, sizeof (HostName), NULL, 0, 0);
			rc = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in6), IPName, sizeof (IPName), NULL, 0, 1);
			printf("iPv6 : %s -> %s %s\n", ifa->ifa_name, IPName, HostName);
		}
	}

	return 1;
}

int DefaultOnConnect(GlobalServer* pGlobalServer, unsigned long clientSocket) {
	socklen_t len;
	struct sockaddr_in  iPv4Client;
	struct sockaddr_in6 iPv6Client;
	char str[INET6_ADDRSTRLEN];

	if (memcmp(pGlobalServer->IPVersion, "IPv4",  4) == 0) {
		len = sizeof(struct sockaddr_in);
		getpeername(clientSocket, (struct sockaddr *)&iPv4Client, &len);

		if(inet_ntop(AF_INET, &iPv4Client.sin_addr, str, sizeof(str))) {
		  printf("Client address is %s\n", str);
		  printf("Client port is %d\n", ntohs(iPv4Client.sin_port));
		}
	}
	else {
		len = sizeof(struct sockaddr_in6);
		getpeername(clientSocket, (struct sockaddr *)&iPv6Client, &len);

		if(inet_ntop(AF_INET6, &iPv6Client.sin6_addr, str, sizeof(str))) {
		  printf("Client address is %s\n", str);
		  printf("Client port is %d\n", ntohs(iPv6Client.sin6_port));
		}
	}

	send(clientSocket, "Welcome...\n", strlen("Welcome...\n"), 0);
	send(clientSocket, "Give your prompt : \n", strlen("Welcome..."), 0);

	return 1;
}

int DefaultDispatchServer(GlobalServer* pGlobalServer) {
	fd_set read_fds, master_fds;
	unsigned long clientSocket, i, j, fds_max;
	char Buffer[81];
	int rc;

	FD_ZERO (&master_fds);
    FD_SET (pGlobalServer->sockDesc, &master_fds);
	fds_max = pGlobalServer->sockDesc;

    pGlobalServer->pArtiComm->master_fds = master_fds;
	while(1) {

		read_fds = master_fds;
		rc = select (fds_max + 1, &read_fds, NULL, NULL, (struct timeval *)NULL);
        if (rc <0) {
                printf("Error Select Socket : %d\n", rc);
                return 0;
        }

        for(i = 0; i <= fds_max; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == pGlobalServer->sockDesc) {
                    clientSocket = accept(pGlobalServer->sockDesc, NULL, NULL);
                    if (clientSocket < 0) {
                        printf("Error Accepting Socket : %d\n", rc);
                        return 0;
                    } else {

                        FD_SET(clientSocket, &master_fds);
                        if (clientSocket > fds_max) fds_max = clientSocket;
                        pGlobalServer->pArtiComm->master_fds = master_fds;

                        rc = pGlobalServer->pPHandler->pOnConnect(pGlobalServer, clientSocket);
                    }
                } else {

                    pGlobalServer->pArtiComm->master_fds = master_fds;

                    rc = pGlobalServer->pPHandler->pRunning(pGlobalServer, fds_max, i);
                    /**
                    memset(Buffer, 0, 81);
                    if ((rc = recv(i, Buffer, 80, 0)) <= 0) {
                        if (rc == 0) {
                            rc = pGlobalServer->pPHandler->pOnDisconnect(pGlobalServer, i);
                        }

                        close(i);
                        FD_CLR(i, &master_fds);
                    } else {

                        for(j = 0; j <= fds_max; j++) {
                            if (FD_ISSET(j, &master_fds)) {
                                if (j != pGlobalServer->sockDesc && j != i) {
                                    if (send(j, Buffer, rc, 0) == -1) {
                                                        printf("Error Sending Information\n");
                                                        return 0;
                                    }
                                }
                            }
                        }
                    }                    */
                }
            }
        }
	}

	return 1;
}

int EchoServer(GlobalServer* pGlobalServer, unsigned long fds_max, unsigned long clientSocket) {
    fd_set master_fds;
    char Buffer[81];
    int j;
    int rc;

    master_fds = pGlobalServer->pArtiComm->master_fds ;

#ifdef AIP_VIEW_DEBUG
    printf("Entring EchoServer\n");
#endif
    memset(Buffer, 0, 81);
    //ReadMessageFromSocket();
    if ((rc = recv(clientSocket, Buffer, 80, 0)) <= 0) {
        if (rc == 0) {
            rc = pGlobalServer->pPHandler->pOnDisconnect(pGlobalServer, clientSocket);
        }

        close(clientSocket);
        FD_CLR(clientSocket, &master_fds);
    } else {

        for(j = 0; j <= fds_max; j++) {
            if (FD_ISSET(j, &master_fds)) {
                if (j != pGlobalServer->sockDesc && j != clientSocket) {
#ifdef AIP_VIEW_DEBUG
                    printf("Send ! j (%d) -> i (%d)\n", clientSocket, j);
#endif
                    if (send(j, Buffer, rc, 0) == -1) {
                        printf("Error Sending Information\n");
                        return 0;
                    }
                }
            }
        }
    }

    pGlobalServer->pArtiComm->master_fds = master_fds;

    return rc;
}

int DefaultOnDisconnect(GlobalServer* pGlobalServer, unsigned long clientSocket) {

	printf("Closing Connection for socket %ld...\n", clientSocket);
	return 1;
}

