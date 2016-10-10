#include "networks.h"


List* GetNetworksList(struct ifaddrs *ifaddr) {
    List* plifCfg;
    struct ifaddrs *ifa;
    struct sockaddr *sa;
    struct ifreq ifr;
    int fd, family, i, rc;
    ifCfg* pifCfg;

    plifCfg = NULL;

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {

        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;
        if (family == AF_PACKET)
            continue;

        pifCfg = (ifCfg*)malloc(sizeof(ifCfg));
        if (!pifCfg)
            return NULL;

        pifCfg->EthName = (char*)malloc(sizeof(char)*strlen(ifa->ifa_name) + 1);
        if (!pifCfg->EthName)
            return NULL;
        memset(pifCfg->EthName, 0, strlen(ifa->ifa_name) + 1);
        memcpy(pifCfg->EthName, ifa->ifa_name, strlen(ifa->ifa_name));

        if (family == AF_INET)
            pifCfg->Type = IPV4;
        else
            pifCfg->Type = IPV6;

       if (family == AF_INET || family == AF_INET6) {

            pifCfg->iNet = (char*)malloc(sizeof(char)*((family == AF_INET) ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN) + 1);
            if (!pifCfg->iNet)
                return NULL;
            memset(pifCfg->iNet, 0, sizeof(char)*((family == AF_INET) ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN) + 1);

            sa = (struct sockaddr *)ifa->ifa_addr;
            if (sa) {
                inet_ntop((family == AF_INET) ? AF_INET : AF_INET6,
                          (family == AF_INET) ? (void*)&(((struct sockaddr_in *)sa)->sin_addr) : (void*)&(((struct sockaddr_in6 *)sa)->sin6_addr),
                          pifCfg->iNet,
                          (family == AF_INET) ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN);
            }
            else
                memcpy(pifCfg->iNet, "<->", 3);

            pifCfg->iMsk = (char*)malloc(sizeof(char)*((family == AF_INET) ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN) + 1);
            if (!pifCfg->iMsk)
                return NULL;
            memset(pifCfg->iMsk, 0, sizeof(char)*((family == AF_INET) ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN) + 1);

            sa = (struct sockaddr *)ifa->ifa_netmask;
            if (sa) {
                inet_ntop((family == AF_INET) ? AF_INET : AF_INET6,
                          (family == AF_INET) ? (void*)&(((struct sockaddr_in *)sa)->sin_addr) : (void*)&(((struct sockaddr_in6 *)sa)->sin6_addr),
                          pifCfg->iMsk,
                          (family == AF_INET) ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN);
            }
            else
                memcpy(pifCfg->iMsk, "<->", 3);

            pifCfg->bCast = (char*)malloc(sizeof(char)*((family == AF_INET) ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN) + 1);
            if (!pifCfg->bCast)
                return NULL;
            memset(pifCfg->bCast, 0, sizeof(char)*((family == AF_INET) ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN) + 1);

            sa = (struct sockaddr *)ifa->ifa_dstaddr;
            if (sa) {
                inet_ntop((family == AF_INET) ? AF_INET : AF_INET6,
                          (family == AF_INET) ? (void*)&(((struct sockaddr_in *)sa)->sin_addr) : (void*)&(((struct sockaddr_in6 *)sa)->sin6_addr),
                          pifCfg->bCast,
                          (family == AF_INET) ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN);
            }
            else
                memcpy(pifCfg->bCast, "<->", 3);
        }

        fd = socket((family == AF_INET) ? AF_INET : AF_INET6, SOCK_DGRAM, 0);

        memset(&ifr, 0, sizeof(struct ifreq));
        ifr.ifr_addr.sa_family = (family == AF_INET) ? AF_INET : AF_INET6;
        memcpy(ifr.ifr_name, pifCfg->EthName, strlen(pifCfg->EthName));

        ioctl(fd, SIOCGIFHWADDR, &ifr);
        pifCfg->HWaddr = (uchar*)malloc(sizeof(char)*7);
        if (!pifCfg->HWaddr)
            return NULL;
        memset(pifCfg->HWaddr, 0, 7);
        memcpy(pifCfg->HWaddr, ifr.ifr_hwaddr.sa_data, 6);

        ioctl(fd, SIOCGIFMTU, &ifr);
        pifCfg->MTU = ifr.ifr_mtu;

        ioctl(fd, SIOCGIFFLAGS, &ifr);
        pifCfg->Flags = ifr.ifr_flags;
        close(fd);

        AddMemberOnTop(&plifCfg, pifCfg);
    }

    return plifCfg;
}

int ViewEther(List* plifCfg) {
    ifCfg* pifCfg;
    Chaine* pChaine;

    pChaine = plifCfg->Main;

    while (pChaine) {
        pifCfg = (ifCfg*)pChaine->Member;

        printf("Eth %s (%s) - IP %s iMsk %s bCast %s\n", pifCfg->EthName, (pifCfg->Type == IPV4) ? "IPv4" : "IPv6", pifCfg->iNet, pifCfg->iMsk, pifCfg->bCast);
        printf("\t M@c %.2x:%.2x:%.2x:%2x:%.2x:%.2x - MTU %d\n", pifCfg->HWaddr[0], pifCfg->HWaddr[1], pifCfg->HWaddr[2], pifCfg->HWaddr[3], pifCfg->HWaddr[4], pifCfg->HWaddr[5],
                pifCfg->MTU);

        pChaine = pChaine->Next;
    }


    return 1;
}

char* GetIPFromEthName(List* plifCfg, char* EthName, IPFamily IPType) {
    ifCfg* pifCfg;
    Chaine* pChaine;

    pChaine = plifCfg->Main;

    while (pChaine) {
        pifCfg = (ifCfg*)pChaine->Member;

        if ((IPType == pifCfg->Type)
            && (memcmp(EthName, pifCfg->EthName, strlen(pifCfg->EthName)) == 0)
            && (strlen(EthName) == strlen(pifCfg->EthName)))
            return pifCfg->iNet;

        pChaine = pChaine->Next;
    }

    Checkpoint;
    return NULL;
}

Signature* InitSignature(char* HName, Uuid* pUuid) {
    Signature* pSign;
	struct utsname my_uname;
	int rc;
	uint Lng;

    pSign = (Signature*)malloc(sizeof(Signature));
    if(!pSign)
        return NULL;

    GenerateUUID(pUuid);
    pSign->UId = GetUUIDString(pUuid);

    pSign->HName = (char*)malloc(sizeof(char)*strlen(HName) + 1);
    if (!pSign->HName)
        return NULL;
    memset(pSign->HName, 0, sizeof(char)*strlen(HName) + 1);
    memcpy(pSign->HName, HName, strlen(HName));

	uname(&my_uname);
	Lng = strlen(my_uname.sysname) + strlen(my_uname.nodename) + strlen(my_uname.release)
		+ strlen(my_uname.version) + strlen(my_uname.machine) + 5;
	pSign->OSDesc = (char*)malloc(Lng*sizeof(char) + 1);
	if (!pSign->OSDesc)
        return NULL;
    sprintf(pSign->OSDesc, "%s %s %s %s %s", my_uname.sysname, my_uname.nodename, my_uname.release,my_uname.version,my_uname.machine);

	rc = getifaddrs(&(pSign->ifAddress));
	if (rc < 0)
		return NULL;

    pSign->plifCfg = GetNetworksList(pSign->ifAddress);

    ViewEther(pSign->plifCfg);

    return pSign;
}

/**
    Connect's structure function related     */
Connect* CreateConnector(Networks* pNetworks, char* HName, unsigned int LPort, IPFamily IPType, Protocol PPType) {
    Connect* pConnect;
    struct addrinfo Hints;
    struct addrinfo *pServer;
    char HPort[10];
    int rc;

    pConnect = (Connect*)malloc(sizeof(Connect));
    if (!pConnect)
        return NULL;

    pConnect->IPType = IPType;
    pConnect->PPType = PPType;

    pConnect->Port = LPort;
    if (HName) {
        pConnect->HostName = (char*)malloc(sizeof(char)*strlen(HName) + 1);
        if (!pConnect->HostName)
            return NULL;

        memset(pConnect->HostName, 0, sizeof(char)*strlen(HName) + 1);
        memcpy(pConnect->HostName, HName, sizeof(char)*strlen(HName));
    }
    else
        pConnect->HostName = NULL;

    pConnect->pComm = NULL;

    /**
        First create a socket */
    pConnect->clientSocket = socket((IPType == IPV4) ? AF_INET : AF_UNSPEC, (PPType == TCP) ? SOCK_STREAM : SOCK_DGRAM, 0);
    if (pConnect->clientSocket <= 0)
        return NULL;

    pConnect->isConnect = false;

    /**
        Get Host server informations */
    memset(&Hints, 0, sizeof(Hints));
    Hints.ai_family = (IPType == IPV4) ? AF_INET : AF_UNSPEC;
    Hints.ai_socktype = (PPType == TCP) ? SOCK_STREAM : SOCK_DGRAM;

    memset(HPort, 0, 10);
    sprintf(HPort, "%d", LPort);

    if (getaddrinfo(HName, HPort, &Hints, &pServer) != 0)
        return NULL;

    pConnect->IP_Addr = (struct sockaddr*)malloc((IPType == IPV4) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6));
    if (!pConnect->IP_Addr)
        return NULL;

    memcpy(pConnect->IP_Addr, (struct sockaddr *) pServer->ai_addr, (IPType == IPV4) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6));

    freeaddrinfo(pServer);

    AddMemberOnTop(&pNetworks->plConnect, pConnect);
    pConnect->pNetworks = pNetworks;

    pNetworks->nbSockets++;
    pNetworks->AllSockets[pNetworks->nbSockets - 1] = pConnect->clientSocket;

    pNetworks->nbClients++;
    pNetworks->AllClients[pNetworks->nbClients - 1] = pConnect->clientSocket;

    return pConnect;
}


/**
    Listener's structure function related     */
Listener* CreateListener(Networks* pNetworks, char* EthName, unsigned long LPort, IPFamily IPType, Protocol PPType) {
    Listener* pListener;
    struct sockaddr_in  IP4_Addr;
    struct sockaddr_in6 IP6_Addr;
    int rc, OptVal;

    pListener = (Listener*)malloc(sizeof(Listener));
    if (!pListener)
        return NULL;

    pListener->IPType = IPType;
    pListener->PPType = PPType;

    pListener->Port = LPort;
    if (EthName) {
        pListener->EthName = (char*)malloc(sizeof(char)*strlen(EthName) + 1);
        if (!pListener->EthName)
            return NULL;

        memset(pListener->EthName, 0, sizeof(char)*strlen(EthName) + 1);
        memcpy(pListener->EthName, EthName, sizeof(char)*strlen(EthName));
    }
    else
        pListener->EthName = NULL;

    pListener->plCommunication = NULL;
    pListener->plThread = NULL;

    pListener->pNetworks = pNetworks;
    pListener->Stack = DEFAULT_STACK;

    /**
        First create a socket */
    pListener->serverSocket = socket((IPType == IPV4) ? AF_INET : AF_UNSPEC, (PPType == TCP) ? SOCK_STREAM : SOCK_DGRAM, 0);
    if (pListener->serverSocket <= 0)
        return NULL;

    printf("pListener->serverSocket %d\n", pListener->serverSocket);
    OptVal = 1;
    setsockopt (pListener->serverSocket, SOL_SOCKET, SO_REUSEADDR, &OptVal, sizeof (OptVal));

    /**
        Second bind this socket */
    if (IPType == IPV4) {
        IP4_Addr.sin_family = AF_INET;
        IP4_Addr.sin_port = htons(LPort);

        if (EthName)
            inet_pton(AF_INET, GetIPFromEthName(pNetworks->pSign->plifCfg, EthName, IPV4), &IP4_Addr.sin_addr);
        else
            IP4_Addr.sin_addr.s_addr = INADDR_ANY;
        rc = bind(pListener->serverSocket, (struct sockaddr *)&IP4_Addr, sizeof(struct sockaddr_in));
    }
    else {
        IP6_Addr.sin6_family = AF_INET6;
        IP6_Addr.sin6_port = htons(LPort);

        if (EthName)
            inet_pton(AF_INET6, GetIPFromEthName(pNetworks->pSign->plifCfg, EthName, IPV6), &IP6_Addr.sin6_addr);
        else
            memcpy(IP6_Addr.sin6_addr.s6_addr, &in6addr_any, sizeof(in6addr_any));
        rc = bind(pListener->serverSocket, (struct sockaddr *)&IP6_Addr, sizeof(struct sockaddr_in6));
    }

    if (rc != 0)
        return NULL;

    listen(pListener->serverSocket, pListener->Stack);

    AddMemberOnTop(&pNetworks->plListen, pListener);
    pListener->pNetworks = pNetworks;

    pNetworks->nbSockets++;
    pNetworks->AllSockets[pNetworks->nbSockets - 1] = pListener->serverSocket;

    pNetworks->nbListening++;
    pNetworks->AllListening[pNetworks->nbListening - 1] = pListener->serverSocket;

    return pListener;
}

Listener* GetListenerFromSocket(Networks* pNetworks, uint Socket) {
    Listener* pListener;
    Chaine* pChaine;

    pChaine = (Chaine* )pNetworks->plListen->Main;

    while (pChaine) {
        pListener = (Listener*)pChaine->Member;

        if (pListener->serverSocket == Socket)
            return pListener;

        pChaine = pChaine->Next;
    }

    return NULL;
}

/**
    Communication's structure function related     */
Communication* CreateCommunicationServer(Networks* pNetworks, Listener* pListener, uint clientSocket, struct sockaddr* IP) {
    Communication* pComm;
    ProtocolHandler* pPHandler;

    pComm = (Communication*)malloc(sizeof(Communication));
    if (!pComm)
        return NULL;

    pComm->CommCh = SERVER;
    pComm->IPType = pListener->IPType;
    pComm->PPType = pListener->PPType;

    pComm->Socket = clientSocket;

    pComm->pNetworks = pNetworks;

    pComm->pConnect = NULL;
    pComm->pListener = pListener;

    pPHandler = pListener->pPHandler;
    pComm->pPHandler = pPHandler;

    pComm->Init = NULL;

    pComm->Recv = (uchar*)malloc(sizeof(uchar)*pPHandler->packetSize);
    if (!pComm->Recv)
        return NULL;

    pComm->Send = (uchar*)malloc(sizeof(uchar)*pPHandler->packetSize);
    if (!pComm->Send)
        return NULL;

    pComm->Recvlen = 0;
    pComm->Sendlen = 0;
    pComm->Initlen = 0;

    pComm->IP_Addr = (struct sockaddr*)malloc((pComm->IPType == IPV4) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6));
    if (!pComm->IP_Addr)
        return NULL;

    memcpy(pComm->IP_Addr, (struct sockaddr *) IP, (pComm->IPType == IPV4) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6));

    pComm->pPHandler = pListener->pPHandler;

    pComm->CTime = time(NULL);

    pComm->pThread = (Thread*)InitThread(pComm, "Communication Process", FreeCommunicationThread);
    pComm->pThread->pBeginningThread = (ThreadRunning*)StartCommunication;
    pComm->pThread->pRunningThread = (ThreadRunning*)LoopCommunication;
    pComm->pThread->pEndingThread = (ThreadRunning*)StopCommunication;

    return pComm;
}

Communication* CreateCommunicationClient(Networks* pNetworks, Connect* pConnect) {
    Communication* pComm;
    ProtocolHandler* pPHandler;

    pComm = (Communication*)malloc(sizeof(Communication));
    if (!pComm)
        return NULL;

    pComm->CommCh = CLIENT;
    pComm->IPType = pConnect->IPType;
    pComm->PPType = pConnect->PPType;

    pComm->Socket = pConnect->clientSocket;

    pComm->pNetworks = pNetworks;
    pComm->pConnect = pConnect;
    pComm->pListener = NULL;

    pPHandler = pConnect->pPHandler;
    pComm->pPHandler = pPHandler;

    pComm->Init = NULL;

    pComm->Recv = (uchar*)malloc(sizeof(uchar)*pPHandler->packetSize);
    if (!pComm->Recv)
        return NULL;

    pComm->Send = (uchar*)malloc(sizeof(uchar)*pPHandler->packetSize);
    if (!pComm->Send)
        return NULL;

    pComm->Recvlen = 0;
    pComm->Sendlen = 0;
    pComm->Initlen = 0;

    pComm->IP_Addr = (struct sockaddr*)malloc((pComm->IPType == IPV4) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6));
    if (!pComm->IP_Addr)
        return NULL;

    memcpy(pComm->IP_Addr, (struct sockaddr *) pConnect->IP_Addr, (pComm->IPType == IPV4) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6));

    pComm->pPHandler = pConnect->pPHandler;

    pComm->CTime = time(NULL);

    pComm->pThread = (Thread*)InitThread(pComm, "Communication Process", FreeCommunicationThread);
    pComm->pThread->pBeginningThread = (ThreadRunning*)StartCommunication;
    pComm->pThread->pRunningThread = (ThreadRunning*)LoopCommunication;
    pComm->pThread->pEndingThread = (ThreadRunning*)StopCommunication;

    return pComm;
}

void FreeCommunicationThread(void* pMemory) {
    Communication* pComm;
    Networks* pNetworks;
    Listener* pListener;

    printf("FreeCommunication ...\n");

    pComm = (Communication*)pMemory;
    pListener = (Listener*)pComm->pListener;
    pNetworks = (Networks*)pComm->pNetworks;

    free(pComm->Send);
    free(pComm->Recv);

    free(pComm->IP_Addr);
    SimpleRemoveMember(&pListener->plCommunication, pComm);
    SimpleRemoveMember(&pListener->plThread, pComm->pThread);
    SimpleRemoveMember(&pNetworks->pTaskManager->plThread, pComm->pThread);

    free(pComm);
}

Communication* GetCommunicationFromSockaddr(List* plCommunication, IPFamily IPType, struct sockaddr * IP_Addr) {
    Communication* pComm;
    Chaine* pChaine;

    if (!plCommunication)
        return NULL;

    pChaine = (Chaine *)plCommunication->Main;

    while (pChaine) {
        pComm = (Communication* )pChaine->Member;

        if (memcmp(pComm->IP_Addr, IP_Addr, (pComm->IPType == IPV4) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6)) == 0)
            return pComm;

        pChaine = pChaine->Next;
    }

    return NULL;
}

int CleanCommunicationTimer(Listener* pListener) {
    Communication* pComm;
    List* plCommunication;
    Chaine* pChaine;
    int rc;

    plCommunication = (List*)pListener->plCommunication;

    if (!plCommunication)
        return 0;

    pChaine = (Chaine *)plCommunication->Main;

    while (pChaine) {
        pComm = (Communication* )pChaine->Member;

        if ((uint)time(NULL) - (uint)pComm->CTime >= pComm->pPHandler->TimeOut) {
            Checkpoint;
            rc = pComm->pPHandler->pOnDisconnect(pComm);
            pComm->isClosed = true;
            pComm->pThread->RunningState = false;
            if (pComm->pThread->Paused)
                ResumeThread(pComm->pThread);
        }

        pChaine = pChaine->Next;
    }

    return 1;
}

int StartCommunication(Thread* pThread) {
    Communication* pComm;
    ProtocolHandler* pPHandler;
    socklen_t AddrLen;
    int rc;

    pComm = (Communication* )pThread->pThreadData;
    pPHandler = (ProtocolHandler* )pComm->pPHandler;
    pThread->RunningState = true;

    rc = pComm->pPHandler->pOnConnect(pComm);
    pComm->isClosed = false;

    if (pComm->Sendlen > 0) {
        if (pComm->PPType == TCP)
            rc = send(pComm->Socket, pComm->Send, pComm->Sendlen, 0);
        else
            rc = sendto(pComm->Socket, pComm->Send, pComm->Sendlen, 0, pComm->IP_Addr,
                      (pComm->IPType == IPV4) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6));

        pComm->Sendlen = 0;
        memset(pComm->Send, 0, pPHandler->packetSize);
    }

    return 1;
}

int LoopCommunication(Thread* pThread) {
    Communication* pComm;
    ProtocolHandler* pPHandler;
    socklen_t AddrLen;
    int rc;

    pComm = (Communication* )pThread->pThreadData;
    pPHandler = (ProtocolHandler* )pComm->pPHandler;

    pComm->CTime = time(NULL);

    if (pComm->PPType == TCP) {
        rc = recv(pComm->Socket, pComm->Recv, pPHandler->packetSize, 0);
        if (rc <= 0) {
            if (rc == 0) {
                rc = pPHandler->pOnDisconnect(pComm);
                pComm->isClosed = true;
                pThread->RunningState = false;
            }
        }
        else
            pComm->Recvlen = rc;
    }
    else {
        if (pComm->Recvlen == 0)
            YieldThread(pThread);
    }

    if (!pComm->isClosed)
        rc = pComm->pPHandler->pSLoop(pComm);

    if (pComm->Sendlen > 0) {
        if (pComm->PPType == TCP)
            rc = send(pComm->Socket, pComm->Send, pComm->Sendlen, 0);
        else
            rc = sendto(pComm->Socket, pComm->Send, pComm->Sendlen, 0, pComm->IP_Addr,
                      (pComm->IPType == IPV4) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6));

        pComm->Sendlen = 0;
        memset(pComm->Send, 0, pPHandler->packetSize);
    }

    return 1;
}

int StopCommunication(Thread* pThread) {
    Communication* pComm;
    Networks* pNetworks;
    Listener* pListener;
    int i;

    pComm = (Communication*)pThread->pThreadData;
    pListener = (Listener*)pComm->pListener;
    pNetworks = (Networks*)pComm->pNetworks;

    /* Clean */
    if (pComm->PPType = TCP) {
        for (i = 0; i <= pNetworks->nbClients; i++)
            if (pNetworks->AllClients[i] == pComm->Socket)
                pNetworks->AllClients[i] = 0;

        ReduceArray(pNetworks->AllClients,  pNetworks->nbClients);
        pNetworks->nbClients = SizeArray(pNetworks->AllClients, pNetworks->nbClients);

        for (i = 0; i <= pListener->nbSockets; i++)
            if (pListener->connectSocket[i] == pComm->Socket)
                pListener->connectSocket[i] = 0;

        ReduceArray(pListener->connectSocket, pListener->nbSockets);
        pListener->nbSockets = SizeArray(pListener->connectSocket, pListener->nbSockets);

        close(pComm->Socket);
    }

    FreeThread(pThread);

    return 1;
}

/**
    ProtocolHandler's structure function related     */
int SetEchoProtocol(Listener* pListener) {
    ProtocolHandler* pPHandler;

    pPHandler = (ProtocolHandler*)malloc(sizeof(ProtocolHandler));
    if (!pPHandler)
        return 0;

    pPHandler->SName = (char*)malloc(sizeof(char)*64);
    if (pPHandler->SName)
    memset(pPHandler->SName, 0, 64);

    strcpy(pPHandler->SName, "ECHO Server V 1.0 - AIP%");

    pPHandler->LongDesc = (char*)malloc(sizeof(char)*Max256);
    if (pPHandler->LongDesc)
    memset(pPHandler->LongDesc, 0, Max256);

    strcpy(pPHandler->LongDesc, "Welcome to an Echo Server implementation for AIP% framework");

    pPHandler->packetSize = DEFAULT_PACKET;
    pPHandler->TimeOut = DEFAULT_TIMER;
    pPHandler->pOnConnect = OnConnect_EchoProtocol;
    pPHandler->pOnDisconnect = OnDisconnect_EchoProtocol;
    pPHandler->pOnRecv = NULL;
    pPHandler->pOnSend = NULL;
    pPHandler->pData = NULL;
    pPHandler->pSLoop = ServerLoop_EchoProtocol;

    pListener->pPHandler = pPHandler;

    return 1;
}

int OnConnect_EchoProtocol(void* pMemory) {
    Communication* pComm;

    pComm = (Communication *)pMemory;

    memcpy(pComm->Send, pComm->pPHandler->SName, strlen(pComm->pPHandler->SName));
    pComm->Sendlen = strlen(pComm->pPHandler->SName);

    memcpy(pComm->Send + pComm->Sendlen, "\n", 1);
    pComm->Sendlen++;

    memcpy(pComm->Send + pComm->Sendlen, pComm->pPHandler->LongDesc, strlen(pComm->pPHandler->LongDesc));
    pComm->Sendlen += strlen(pComm->pPHandler->LongDesc);

    memcpy(pComm->Send + pComm->Sendlen, "\n\n", 2);
    pComm->Sendlen += 2;

    if (strlen(pComm->Recv)) {
        if (pComm->Sendlen + pComm->Recvlen < pComm->pPHandler->packetSize)
            memcpy(pComm->Send + pComm->Sendlen, pComm->Recv, pComm->Recvlen);
        else
            memcpy(pComm->Send + pComm->Sendlen, "Buffer Overflow", 15);

        pComm->Recvlen = 0;
        memset(pComm->Recv, 0, pComm->pPHandler->packetSize);

    }

    return 1;
}

int ServerLoop_EchoProtocol(void* pMemory) {
    Communication* pComm;

    pComm = (Communication *)pMemory;

    memcpy(pComm->Send, pComm->Recv, pComm->Recvlen);
    pComm->Sendlen = pComm->Recvlen;

    pComm->Recvlen = 0;
    memset(pComm->Recv, 0, pComm->pPHandler->packetSize);

    return 1;
}

int OnDisconnect_EchoProtocol(void *pMemory) {

    return 1;
}

int SetUuidProtocol(Listener* pListener) {
    ProtocolHandler* pPHandler;

    pPHandler = (ProtocolHandler*)malloc(sizeof(ProtocolHandler));
    if (!pPHandler)
        return 0;

    pPHandler->SName = (char*)malloc(sizeof(char)*64);
    if (pPHandler->SName)
    memset(pPHandler->SName, 0, 64);

    strcpy(pPHandler->SName, "UUID Server V 1.0 - AIP%");

    pPHandler->LongDesc = (char*)malloc(sizeof(char)*Max256);
    if (pPHandler->LongDesc)
    memset(pPHandler->LongDesc, 0, Max256);

    strcpy(pPHandler->LongDesc, "Welcome to an UUID Server implementation for AIP% framework");

    pPHandler->packetSize = DEFAULT_PACKET;
    pPHandler->TimeOut = DEFAULT_TIMER;
    pPHandler->pOnConnect = OnConnect_UuidProtocol;
    pPHandler->pOnDisconnect = OnDisconnect_UuidProtocol;
    pPHandler->pOnRecv = NULL;
    pPHandler->pOnSend = NULL;
    pPHandler->pData = NULL;
    pPHandler->pSLoop = ServerLoop_UuidProtocol;

    pListener->pPHandler = pPHandler;

    return 1;
}

int OnConnect_UuidProtocol(void* pMemory) {
    Communication* pComm;

    pComm = (Communication *)pMemory;

    memcpy(pComm->Send, pComm->pPHandler->SName, strlen(pComm->pPHandler->SName));
    pComm->Sendlen = strlen(pComm->pPHandler->SName);

    memcpy(pComm->Send + pComm->Sendlen, "\n", 1);
    pComm->Sendlen++;

    memcpy(pComm->Send + pComm->Sendlen, pComm->pPHandler->LongDesc, strlen(pComm->pPHandler->LongDesc));
    pComm->Sendlen += strlen(pComm->pPHandler->LongDesc);

    memcpy(pComm->Send + pComm->Sendlen, "\n\n", 2);
    pComm->Sendlen += 2;

    if (strlen(pComm->Recv)) {
        if (pComm->Sendlen + pComm->Recvlen < pComm->pPHandler->packetSize)
            memcpy(pComm->Send + pComm->Sendlen, pComm->Recv, pComm->Recvlen);
        else
            memcpy(pComm->Send + pComm->Sendlen, "Buffer Overflow", 15);

        pComm->Recvlen = 0;
        memset(pComm->Recv, 0, pComm->pPHandler->packetSize);

    }

    return 1;
}

int ServerLoop_UuidProtocol(void* pMemory) {
    Communication* pComm;
    Uuid* pUuid;
    char* pTmp;
    uint NbUuid, i;

    pComm = (Communication *)pMemory;

    NbUuid = atol(pComm->Recv);
    pUuid = (Uuid* )pComm->pNetworks->pUuid;

    if ((NbUuid >= 32) || (NbUuid <= 0))
        NbUuid = 1;
    for (i = 0; i < NbUuid; i++) {
        GenerateUUID(pUuid);
        pTmp = GetUUIDString(pUuid);
        printf("%s\n", pTmp);
        memcpy(pComm->Send + pComm->Sendlen, pTmp, strlen(pTmp));
        pComm->Sendlen += strlen(pTmp);
        memcpy(pComm->Send + pComm->Sendlen, "\n", 1);
        pComm->Sendlen++;

        free(pTmp);
    }

    pComm->Recvlen = 0;
    memset(pComm->Recv, 0, pComm->pPHandler->packetSize);

    return 1;
}

int OnDisconnect_UuidProtocol(void *pMemory) {

    return 1;
}

int LoadHTTPCrawler(Connect* pConnect) {
    ProtocolHandler* pPHandler;

    pPHandler = (ProtocolHandler*)malloc(sizeof(ProtocolHandler));
    if (!pPHandler)
        return 0;

    pPHandler->SName = (char*)malloc(sizeof(char)*64);
    if (pPHandler->SName)
    memset(pPHandler->SName, 0, 64);

    strcpy(pPHandler->SName, "HTTP Client V 1.0 - AIP%");

    pPHandler->LongDesc = (char*)malloc(sizeof(char)*Max256);
    if (pPHandler->LongDesc)
    memset(pPHandler->LongDesc, 0, Max256);

    strcpy(pPHandler->LongDesc, "Simple HTTP Client implementation for AIP% framework");

    pPHandler->packetSize = DEFAULT_PACKET;
    pPHandler->TimeOut = DEFAULT_TIMER;
    pPHandler->pOnConnect = OnConnect_HTTPLoad;
    pPHandler->pOnDisconnect = OnDisconnect_HTTPLoad;
    pPHandler->pOnRecv = NULL;
    pPHandler->pOnSend = NULL;
    pPHandler->pData = NULL;
    pPHandler->pSLoop = ServerLoop_HTTPLoad;

    pConnect->pPHandler = pPHandler;

    return 1;
}

int OnConnect_HTTPLoad(void* pMemory) {
    Communication* pComm;
    uchar* Request;

    pComm = (Communication *)pMemory;

    /**
        GET /%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nConnection: close\r\n\r\n */
    sprintf(pComm->Send, "GET / HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nConnection: close\r\n\r\n", pComm->pConnect->HostName, pComm->pPHandler->SName);
    pComm->Sendlen = strlen(pComm->Send);

    return 1;
}

int ServerLoop_HTTPLoad(void* pMemory) {
    Communication* pComm;

    pComm = (Communication *)pMemory;

    memcpy(pComm->Send, pComm->Recv, pComm->Recvlen);
    pComm->Sendlen = pComm->Recvlen;

    pComm->Recvlen = 0;
    memset(pComm->Recv, 0, pComm->pPHandler->packetSize);

    return 1;
}

int OnDisconnect_HTTPLoad(void *pMemory) {

    return 1;
}

/**
    Networks's structure function related     */
Networks* InitNetwoks(char* HName) {
    Networks * pNetworks;
    int  i;

    pNetworks = (Networks*)malloc(sizeof(Networks));
    if (!pNetworks)
        return NULL;

    pNetworks->pUuid = SetUUID();

    pNetworks->plConnect = NULL;
    pNetworks->plListen = NULL;

    FD_ZERO(&pNetworks->Master_Desc);
    pNetworks->SockMax_Desc = 0;

    pNetworks->nbSockets = 0;
    pNetworks->nbClients = 0;
    pNetworks->nbListening = 0;

    for (i = 0; i < Max1024; i++) {
        pNetworks->AllSockets[i] = 0;
        pNetworks->AllListening[i] = 0;
        pNetworks->AllClients[i] = 0;
    }

    pNetworks->pThread = NULL;
    pNetworks->pTaskManager = NULL;

    pNetworks->pSign = (Signature *)InitSignature(HName, pNetworks->pUuid);
    if (!pNetworks->pSign)
        return NULL;

    pNetworks->pThread = (Thread*)InitThread(pNetworks, "Main Networks Process", NULL);
    pNetworks->pThread->pBeginningThread = (ThreadRunning*)NetworksBeginning;
    pNetworks->pThread->pRunningThread = (ThreadRunning*)NetworksDispatch;
    pNetworks->pThread->pEndingThread = (ThreadRunning*)NetworksEnding;

    return pNetworks;
}


int NetworksBeginning(Thread* pThread) {
    Networks* pNetworks;
    Connect* pConnect;
    Communication* pComm;
    Chaine* pChaine;
    int i, rc;

    pNetworks = (Networks*)pThread->pThreadData;

    printf("pNetworks->nbListening %d\n", pNetworks->nbListening);
    FD_ZERO(&(pNetworks->Master_Desc));
    for (i = 0; i < pNetworks->nbListening; i++) {
        FD_SET(pNetworks->AllListening[i] , &(pNetworks->Master_Desc));
        if (pNetworks->AllListening[i] > pNetworks->SockMax_Desc)
            pNetworks->SockMax_Desc = pNetworks->AllListening[i];
    }

    printf("pNetworks->SockMax_Desc  %d\n", pNetworks->SockMax_Desc );

    pThread->RunningState = true;
    pNetworks->isRunning = true;


   if (pNetworks->plConnect)
        return 0;

    pChaine = (Chaine *)pNetworks->plConnect;

    while (pChaine) {
        pConnect = (Connect* )pChaine->Member;

        if (pConnect->PPType == TCP) {

            rc = connect(pConnect->clientSocket, (struct sockaddr *)pConnect->IP_Addr, (pComm->IPType == IPV4) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6));
            if (rc == 0) {
                pConnect->isConnect = true;
                pComm = CreateCommunicationClient(pNetworks, pConnect);
                if (pComm) {
                    StartThread(pNetworks->pTaskManager, pComm->pThread);
                    pConnect->pComm = pComm;
                    pConnect->pThread = NULL;
                }
            }
        }
        else {
            pConnect->isConnect = true;
            pComm = CreateCommunicationClient(pNetworks, pConnect);
            if (pComm){
                StartThread(pNetworks->pTaskManager, pComm->pThread);
                pConnect->pComm = pComm;
                pConnect->pThread = NULL;
            }
        }

        pChaine = pChaine->Next;
    }

    return 1;
}

int NetworksDispatch(Thread* pThread) {
    Networks* pNetworks;
    Communication* pComm;
    Listener* pListener;
    List* pList;
    struct sockaddr_in  IP4_Addr;
    struct sockaddr_in6 IP6_Addr;
    socklen_t AddrLen;
    uint clientSocket;
    uchar* pTmp;
	int i, rc;

    pNetworks = (Networks*)pThread->pThreadData;

    while(pNetworks->isRunning) {

        /**
            Wait for incoming connexion */
		rc = select (pNetworks->SockMax_Desc + 1, &(pNetworks->Master_Desc), NULL, NULL, (struct timeval *)NULL);
        for(i = 0; i < pNetworks->nbListening ; i++) {

            if (FD_ISSET(pNetworks->AllListening[i], &(pNetworks->Master_Desc))) {

                pListener = GetListenerFromSocket(pNetworks, pNetworks->AllListening[i]);
                if (pListener) {
                    if (pListener->PPType == TCP) {
                        AddrLen = (pListener->IPType == IPV4) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
                        clientSocket = accept(pListener->serverSocket, (pListener->IPType == IPV4) ? (struct sockaddr *)&IP4_Addr : (struct sockaddr *)&IP6_Addr, &AddrLen);
                        printf("clientSocket = accept %d\n", clientSocket);
                        if (clientSocket < 0)
                            continue;

                        /** Create Comm Thread from Listener */
                        pNetworks->nbClients++;
                        pNetworks->AllClients[pNetworks->nbClients - 1] = clientSocket;

                        pListener->nbSockets++;
                        pNetworks->AllSockets[pNetworks->nbSockets - 1] = clientSocket;

                        pComm = (Communication* )CreateCommunicationServer(pNetworks, pListener, clientSocket,
                                                                          (pListener->IPType == IPV4) ? (struct sockaddr *)&IP4_Addr : (struct sockaddr *)&IP6_Addr);
                        StartThread(pNetworks->pTaskManager, pComm->pThread);
                        AddMemberOnTop(&pListener->plCommunication, pComm);
                        AddMemberOnTop(&pListener->plThread, pComm->pThread);
                    }
                    else {

                        pTmp = (uchar *)malloc(sizeof(uchar)*DEFAULT_PACKET);
                        if (!pTmp)
                            return 0;
                        memset(pTmp, 0, DEFAULT_PACKET);

                        AddrLen = (pListener->IPType == IPV4) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
                        rc = recvfrom(pListener->serverSocket, pTmp, DEFAULT_PACKET, 0, (pListener->IPType == IPV4) ? (struct sockaddr *)&IP4_Addr : (struct sockaddr *)&IP6_Addr, &AddrLen);

                        pComm = (Communication* )GetCommunicationFromSockaddr(pListener->plCommunication, pListener->IPType,
                                                                                (pListener->IPType == IPV4) ? (struct sockaddr *)&IP4_Addr : (struct sockaddr *)&IP6_Addr);

                        if (!pComm) {
                            pComm = (Communication* )CreateCommunicationServer(pNetworks, pListener, pListener->serverSocket,
                                                                                (pListener->IPType == IPV4) ? (struct sockaddr *)&IP4_Addr : (struct sockaddr *)&IP6_Addr);

                            memcpy(pComm->Recv, pTmp, rc);
                            pComm->Recvlen = rc;
                            free(pTmp);

                            StartThread(pNetworks->pTaskManager, pComm->pThread);
                            AddMemberOnTop(&pListener->plCommunication, pComm);
                            AddMemberOnTop(&pListener->plThread, pComm->pThread);
                        }
                        else {
                            if (rc) {
                                memcpy(pComm->Recv, pTmp, rc);
                                pComm->Recvlen = rc;
                                free(pTmp);
                                ResumeThread(pComm->pThread);
                            }
                        }
                    }
                }
            }
        }

        /** Check for connexion state */
        CleanCommunicationTimer(pListener);

        FD_ZERO(&(pNetworks->Master_Desc));
        for(i = 0; i < pNetworks->nbListening; i++) {
            FD_SET(pNetworks->AllListening[i] , &(pNetworks->Master_Desc));
            if (pNetworks->AllListening[i] > pNetworks->SockMax_Desc)
                pNetworks->SockMax_Desc = pNetworks->AllListening[i];
        }

	}

	return 1;
}


int NetworksEnding(Thread* pThread) {
    Networks* pNetworks;
    int i;

    pNetworks = (Networks*)pThread->pThreadData;

    pNetworks->isRunning = false;

    return 1;
}


int RunNetworks(Networks* pNetworks) {
  TaskManager* pTaskManager;

  pTaskManager = InitTaskManager();

  AddNewThread(pTaskManager, pNetworks->pThread);
  StartingTaskManager(pTaskManager);

  pNetworks->pTaskManager = pTaskManager;

  WaitingTaskManager(pTaskManager);

  return 1;
}
