/*
 *  Command.c
 *  Idiome
 *
 *  Created by Pierre-Jean ALBERT on Tue Apr 08 2014.
 *  Copyright (c) 2013 __An Intellectual Projet__. All rights reserved.
 *
 */

#include "command.h"


Variable* CreateVariable(char* pValue) {
	Variable* pVariable;
  FILE *pFile;
  char* pTmp, *FileName, *pContent;
  unsigned long iReader, Length;

	pVariable = (Variable*)malloc(sizeof(Variable));
	if (!pVariable)
		return NULL;

	pVariable->Content = (char*)malloc(10*sizeof(char));
	memset(pVariable->Content, 0, 10);

	pVariable->Type = 0;

  pFile=fopen(pValue,"rb");
	if (pFile == NULL)
		pVariable->Type = AIP_CHAR;
	else {
		pVariable->Type = AIP_FILE;

		FileName = (char*)malloc(strlen(pValue)*sizeof(char)+ 1);
		if (!FileName)
			return NULL;
		memset(FileName, 0, strlen(pValue)*sizeof(char)+ 1);
		memcpy(FileName, pValue, strlen(pValue));

	  pFile=fopen(FileName,"rb");
	  if (pFile == NULL)
	  	return 0;
	}

	if (pVariable->Type == AIP_FILE) {
		fseek(pFile, 0, SEEK_END);
		Length = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

	  memcpy(pVariable->Content, ":file:", 6);
	  pContent = (char*)realloc(pVariable->Content, 6 + strlen(FileName) + 2 + Length + 1);
		if (!pContent)
			return 0;

	  memcpy(pContent + 6, FileName, strlen(FileName));
	  memcpy(pContent + 6 + strlen(FileName), ":#", 2);

		pTmp = pContent + 6 + strlen(FileName) + 2;
		while (iReader = fread(pTmp, sizeof(char), 65536, pFile))
			pTmp = pTmp + iReader;

		pVariable->Content = pContent;
		pVariable->Length =  strlen(pVariable->Content);

		fclose (pFile);
	}

	if (pVariable->Type == AIP_CHAR) {

	  memcpy(pVariable->Content, ":char:#", 7);
	  pContent = (char*)realloc(pVariable->Content, strlen(pVariable->Content) + strlen(pValue) + 2);
		if (!pContent)
			return 0;
	  memcpy(pContent + 7, pValue, strlen(pValue));

		pVariable->Content = pContent;
		pVariable->Length = strlen(pVariable->Content);
	}

	printf("Variable created (%d) %s\n", pVariable->Length, pVariable->Content);

	return pVariable;
}

Variable* SetAutomateVariable(int Type, char* pMeta, char* pValue) {
	Variable* pVariable;

	pVariable = (Variable*)malloc(sizeof(Variable));
	if (!pVariable)
		return NULL;

	pVariable->Content = (char*)malloc(sizeof(char)*(strlen(pMeta) + strlen(pValue)) + 2);
	memset(pVariable->Content, 0, sizeof(char)*(strlen(pMeta) + strlen(pValue))+ 2);

	pVariable->Type = Type;
  memcpy(pVariable->Content, pMeta, strlen(pMeta));
  memcpy(pVariable->Content+strlen(pMeta), "#", 1);
  memcpy(pVariable->Content+strlen(pMeta)+1, pValue, strlen(pValue));

	pVariable->Length = strlen(pVariable->Content);

	printf("Variable Automate created (%d) %s\n", pVariable->Length, pVariable->Content);

	return pVariable;
}

int FreeVariable(Variable* pVariable) {

	free(pVariable->Content);
	free(pVariable);

	return 1;
}

int AddContenttoCharVariable(Variable* pVariable, char* pValue) {
	unsigned long Length;
	unsigned char* pContent;

	if (pVariable->Type != AIP_CHAR)
		return 0;

	Length = pVariable->Length + strlen(pValue);
	pContent = (unsigned char*)malloc(sizeof(char)*Length + 2);
	memset(pContent, 0, sizeof(char)*Length + 2);
	memcpy(pContent, pVariable->Content, pVariable->Length);

  memcpy(pContent+ pVariable->Length, " ", 1);
  if (pValue[strlen(pValue) - 1] == '"') {
	  memcpy(pContent+pVariable->Length+1, pValue, strlen(pValue)-1);
	}
	else {
	  memcpy(pContent+pVariable->Length+1, pValue, strlen(pValue));
	}

	pVariable->Content = pContent;
	pVariable->Length = strlen(pVariable->Content);

	printf("Variable Add content (%d) %s\n", pVariable->Length, pVariable->Content);

	return 1;
}


unsigned char* GiveContentFromVariable(Variable* pVariable) {
	char* pSave;

	pSave = strstr(pVariable->Content+1, "#");

	return (pVariable->Content+(pVariable->Length-strlen(pSave))) ;
}


Entity* InitEntity(Command* pCommand, char* CmdLine) {
  Entity* pEntity;

  pEntity = (Entity* )malloc(sizeof(Entity));
  if (!pEntity)
    return NULL;

	pEntity->CmdLine = (char*)malloc(strlen(CmdLine)*sizeof(char) + 1);
  memset(pEntity->CmdLine, 0, strlen(CmdLine)*sizeof(char) + 1);
  memcpy(pEntity->CmdLine, CmdLine, strlen(CmdLine));

  pEntity->pCommand = pCommand;
  pEntity->plVariable = NULL;
  pEntity->pOutput = NULL;

  return pEntity;
}

Command* InitCommand(char* pCName, ExecComm* pFunction, char* Cat, char* Xplain, unsigned short nbArg, boolean Block) {
  Command* pCommand;

  pCommand = (Command* )malloc(sizeof(Command));
  if (!pCommand)
    return NULL;

  if (!pCName)
    return NULL;

  pCommand->CName = (char*)malloc(strlen(pCName)*sizeof(char) + 1);
  memset(pCommand->CName, 0, strlen(pCName)*sizeof(char) + 1);
  memcpy(pCommand->CName, pCName, strlen(pCName));

  pCommand->Simple = true;

  if (Xplain) {
    pCommand->Xplain = (char*)malloc(strlen(Xplain)*sizeof(char) + 1);
    memset(pCommand->Xplain, 0, strlen(Xplain)*sizeof(char) + 1);
    memcpy(pCommand->Xplain, Xplain, strlen(Xplain));
  }
  else
    pCommand->Xplain = NULL;

  if (Cat) {
    pCommand->Category = (char*)malloc(strlen(Cat)*sizeof(char) + 1);
    memset(pCommand->Category, 0, strlen(Cat)*sizeof(char) + 1);
    memcpy(pCommand->Category, Cat, strlen(Cat));
  }
  else
    pCommand->Category = NULL;

  pCommand->Block = Block;
  pCommand->pFunction = pFunction;

  pCommand->nbArg = nbArg;

  return pCommand;
}

int FreeCommand(Command* pCommand) {

  free(pCommand->CName);
  if (pCommand->Xplain)
    free(pCommand->Xplain);

  free(pCommand);

  return 1;
}

CentralExec* InitCentralExec(char* PromptName) {
  CentralExec* pCentralExec;

  pCentralExec = (CentralExec* )malloc(sizeof(CentralExec));
  if (!pCentralExec)
    return NULL;

  if (!PromptName)
    return NULL;

  pCentralExec->Prompt = (char*)malloc(strlen(PromptName)*sizeof(char) + 1);
  memset(pCentralExec->Prompt, 0, strlen(PromptName)*sizeof(char) + 1);
  strcpy(pCentralExec->Prompt, PromptName);

  pCentralExec->Continue = true  ;
  pCentralExec->plDictionnary = NULL;

  pCentralExec->Current = -1;
  pCentralExec->History = 0;
  pCentralExec->plHistory = NULL;

  pCentralExec->pThread = (Thread *)InitThread(pCentralExec, "Central.Execute", NULL);
  pCentralExec->pThread->pBeginningThread = (ThreadBeginning *)DefaultCommandStarting;
  pCentralExec->pThread->pRunningThread = (ThreadRunning *)DefaultCommandRunning;
  pCentralExec->pThread->pEndingThread = (ThreadEnding *)DefaultCommandEnding;
  pCentralExec->pThread->pFreeDataFunction = NULL;

  return pCentralExec;
}

void FreeCentralExec(void* pMemory) {
  CentralExec* pCentralExec;
  List* pList;

  pCentralExec = (CentralExec*) pMemory;
  pList = (List*)pCentralExec->plDictionnary;
  if (pList) {
    setFreeFuntion(&pList, (FreeFct*) &FreeCommand);
    FreeList(pList);
    pCentralExec->plDictionnary = NULL;
  }

  free(pCentralExec->Prompt);
  FreeThread(pCentralExec->pThread) ;

  free(pCentralExec);

}

int LoadDefaultCommand(CentralExec* pCentralExec) {

  AddMemberOnTop(&pCentralExec->plDictionnary, InitCommand("date",   Cmd_Date, "Standard", "Give standard Date yyyy-mm-dd", 0, true));
  AddMemberOnTop(&pCentralExec->plDictionnary, InitCommand("time",   Cmd_Time, "Standard", "Give standard time hh:mm:ss", 0, true));
  AddMemberOnTop(&pCentralExec->plDictionnary, InitCommand("help",   Cmd_Help, "Standard", "List Command available", 0, true));
  AddMemberOnTop(&pCentralExec->plDictionnary, InitCommand("quit",   Cmd_Quit, "Standard", "Exit from interactive console", 0, true));
  AddMemberOnTop(&pCentralExec->plDictionnary, InitCommand("host",   Cmd_Host, "Standard", "Give some host information", 0, true));
  AddMemberOnTop(&pCentralExec->plDictionnary, InitCommand("net",    Cmd_Net,  "Standard", "Give some network information", 0, true));

  AddMemberOnTop(&pCentralExec->plDictionnary, InitCommand("md5",    Cmd_Md5,  "Crypto", "Md5 hash FILE or String", 1, true));

/*
  AddMemberOnTop(&pCentralExec->plDictionnary, InitCommand("sha1",   Cmd_Sha1, "Crypto", "Sha1 hash FILE or String", 1, true));

  AddMemberOnTop(&pCentralExec->plDictionnary, InitCommand("crypt",  Cmd_Crypt, "Crypto", "Crypt a string or a file", 2, true));
  AddMemberOnTop(&pCentralExec->plDictionnary, InitCommand("ucrypt", Cmd_Ucrypt, "Crypto", "Uncrypt a string or a file", 2, true));
*/
	return 1;
}

int DefaultCommandStarting(Thread* pThread)
{
  CentralExec* pCentralExec;

	pThread->RunningState = true;
  pCentralExec = (CentralExec *)pThread->pThreadData;

  tcgetattr(STDIN_FILENO, &pCentralExec->Term_OS);

  return 1;
}

int DetectHitKBoard() {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, NULL);
}

int getCharacterFromConsole()
{
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0)
        return r;
    else
        return c;
}


int DefaultCommandRunning(Thread* pThread) {
  CentralExec* pCentralExec;
  struct termios new_termios;
  Entity* pEntity;
  Variable* pVariable;
	unsigned char C;
	char CmdLine[160];
	int i, Crypt;

  pCentralExec = (CentralExec *)pThread->pThreadData;

	memcpy(&new_termios, &pCentralExec->Term_OS, sizeof(new_termios));

  new_termios.c_lflag &= ~ECHO;
  new_termios.c_lflag &= ~ICANON;

  tcsetattr(0, TCSANOW, &new_termios);

  pCentralExec->Continue = true;

  while(pCentralExec->Continue) {
    printf("%s ", pCentralExec->Prompt);
    fflush(stdout);

		i = 0;
		Crypt = 0;
		memset(CmdLine, 0, 120);
		pEntity = NULL;

    do {
    	DetectHitKBoard();
	    C = getCharacterFromConsole();
	    if (C == 27) {
	    	C = getCharacterFromConsole();
	    	if (C == 91) {
		    	C = getCharacterFromConsole();
	    		switch(C) {
	    			case 65:
			    		pEntity = callHistory(pCentralExec, true);
			    		if (pEntity) {
		    				if (i > 0) {
						    	while (i >= 0) {
							    	CmdLine[i] = 0;
						    		printf("\b \b");
						    		i--;
						    	}
						    	printf(" ");
						    }
			    			memcpy(CmdLine, pEntity->CmdLine, strlen(pEntity->CmdLine));
			    			i = strlen(pEntity->CmdLine);
			    			printf("%s", pEntity->CmdLine);
					    	fflush(stdout);
			    		}
				    break;
	    			case 66:
			    		pEntity = callHistory(pCentralExec, false);
			    		if (pEntity) {
		    				if (i > 0) {
						    	while (i >= 0) {
							    	CmdLine[i] = 0;
						    		printf("\b \b");
						    		i--;
						    	}
						    	printf(" ");
						    }
			    			memcpy(CmdLine, pEntity->CmdLine, strlen(pEntity->CmdLine));
			    			i = strlen(pEntity->CmdLine);
			    			printf("%s", pEntity->CmdLine);
					    	fflush(stdout);
			    		}
				    break;
	    			case 67:
	    				if (Crypt)
	    					Crypt = 0;
	    				else
	    					Crypt = 1;
				    	printf("^");
				    	fflush(stdout);
				    break;
	    			case 68:
	    				if (i > 0) {
					    	while (i >= 0) {
						    	CmdLine[i] = 0;
					    		printf("\b \b");
						    	fflush(stdout);
					    		i--;
					    	}
					    	printf(" ");
					    	fflush(stdout);
					    }
				    break;
				    default:
				    	printf("%c\n", C);
				    break;
	    		}
	    	} else
					printf("27 -> %c\n", C);
	    }
	    else {
	    	if (C != 127) {
		    	if (C!=10) {
		    		if (i < 120) {
				    	CmdLine[i] = C;
				    	i++;
				    	if (Crypt)
				    		printf("*");
							else
				    		printf("%c", C);
				    	fflush(stdout);
				    }
				  }
		    }
		    else {
		    	if (i >= 0) {
			    	i--;
			    	CmdLine[i] = 0;
		    		printf("\b \b");
			    	fflush(stdout);
			    }
		    }
	  	}

    } while(C!= 10);

		printf("\n");
		if ((!pEntity) && (strlen(CmdLine) > 1)) {
			pEntity = CreateEntity(pCentralExec, CmdLine);
		}

		if (pEntity) {
			ExecEntity(pCentralExec, pEntity);
			fflush(stdout);
		}
		pEntity = NULL;
  }

  pThread->RunningState = 0;
  return 1;
}

int DefaultCommandEnding(Thread* pThread)
{
  CentralExec* pCentralExec;

  pCentralExec = (CentralExec *)pThread->pThreadData;

  tcsetattr(STDIN_FILENO, TCSANOW, &pCentralExec->Term_OS);

  return 1;
}


Entity *callHistory(CentralExec *pCentralExec, boolean Back) {
	Entity* pEntity;

	pEntity = NULL;

	if (Back)
		pCentralExec->Current++;
	else
		pCentralExec->Current--;

	if (pCentralExec->Current < 0) {
		pCentralExec->Current = -1;
		return NULL;
	}
	if (pCentralExec->Current >= pCentralExec->History) {
		pCentralExec->Current = pCentralExec->History ;
		return NULL;
	}

	if (pCentralExec->Current >= 0)
		pEntity = (Entity*)ReturnMemberAtIndex(pCentralExec->plHistory, pCentralExec->Current);

	return pEntity;
}

int Cmd_Time(void* pStruct, void* pData) {
  CentralExec* pCentralExec;
  time_t timer;
  char pTmp[24];
  struct tm* tm_info;

  pCentralExec = (CentralExec* )pStruct;

  time(&timer);

  memset(pTmp, 0, 24);
  strftime (pTmp, 24, "%H:%M:%S", localtime(&timer));
  printf("Time is %s\n", pTmp);

  return 1;
}

int Cmd_Date(void* pStruct, void* pData) {
  CentralExec* pCentralExec;
  time_t timer;
  char pTmp[24];
  struct tm* tm_info;

  pCentralExec = (CentralExec* )pStruct;

  time(&timer);

  memset(pTmp, 0, 24);
  strftime (pTmp, 24, "%Y-%m-%d", localtime(&timer));
  printf("Date is %s\n", pTmp);

  return 1;
}

int Cmd_Host(void* pStruct, void* pData) {
  CentralExec* pCentralExec;
	struct utsname my_uname;

  pCentralExec = (CentralExec* )pStruct;
  if (!pCentralExec)
  	return 0;

	uname(&my_uname);
  printf("%s %s %s %s %s\n", my_uname.sysname, my_uname.nodename, my_uname.release,my_uname.version,my_uname.machine);

  return 1;
}

int Cmd_Net(void* pStruct, void* pData) {
  CentralExec* pCentralExec;
	struct ifaddrs *ifa, *ifAddress;
  char HostName[512];
  char IPName[128];
  int rc;

  pCentralExec = (CentralExec* )pStruct;

	if (getifaddrs(&ifAddress) < 0)
		return 0;

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

	freeifaddrs(ifAddress);

  return 1;
}

int Cmd_Help(void* pStruct, void* pData) {
  CentralExec* pCentralExec;
  Command* pCommand;
  Chaine* pChaine;

  pCentralExec = (CentralExec* )pStruct;

  printf("Category    Command Name    Explication\n");
  printf("----------  --------------  ----------------\n");
  pChaine = pCentralExec->plDictionnary->Main;
  while (pChaine) {
    pCommand = (Command*)pChaine->Member;

    printf("%-10.10s  %-14.14s  %s\n", pCommand->Category, pCommand->CName, pCommand->Xplain);

    pChaine = pChaine->Next;
  }

  return 1;
}

int Cmd_Md5(void* pStruct, void* pData) {
  CentralExec* pCentralExec;
  Variable* pVariable;
  Entity*   pEntity;
  Chaine* pChaine;
  char Hash[33];
  unsigned char* pContent;

  pCentralExec = (CentralExec* )pStruct;
	pEntity = (Entity*) pData;
  pChaine = (Chaine *)pEntity->plVariable->Main;

  pVariable = (Variable *)pChaine->Member;

	pContent = GiveContentFromVariable(pVariable);
	MD5Hash(pContent, Hash);

	pEntity->pOutput = SetAutomateVariable(AIP_HASH, ":hash:md5:", Hash);

  printf("Md5 is %s\n", Hash);

	return 1;
}

int Cmd_Quit(void* pStruct, void* pData) {
  CentralExec* pCentralExec;

  pCentralExec = (CentralExec* )pStruct;
  printf("Bye bye, have a good night\n");
  pCentralExec->Continue = false;

  return 1;
}

Entity* CreateEntity(CentralExec* pCentralExec, char* CmdLine) {
  Command* pCommand;
  Entity* pEntity;
  Variable* pVariable;
  List* plVariable;
  boolean findCommand, openVariable;
  char *pSave, *pSaveCmd;
  char *pKey;
  int Nb;


  findCommand = false;
  openVariable = false;
  pEntity = NULL;
  plVariable = NULL;
	Nb  = 0;

	pSaveCmd = (char*)malloc(sizeof(char)*strlen(CmdLine)+1);
	memset(pSaveCmd, 0, sizeof(char)*strlen(CmdLine)+1);
	memcpy(pSaveCmd, CmdLine, sizeof(char)*strlen(CmdLine));

  pKey = strtok_r(CmdLine, " \n\r", &pSave);

  do {
    if (!findCommand) {
	    pCommand = FindCommand(pCentralExec->plDictionnary, pKey);
	    if (pCommand) {
	      findCommand = true;
	    }
    }
    else {
    	if (pKey[0] == '"')
    		openVariable = true;

    	if (pCommand->nbArg > 0) {
    		if ((openVariable) && (pKey[0] == '"'))
    			pVariable = (Variable*) CreateVariable(pKey+1);
    		else {
	    		if (!openVariable) {
	    			pVariable = (Variable*) CreateVariable(pKey);
				    AddMemberOnTop(&plVariable, pVariable);
	    			Nb++;
	    		}
	    		else {
	    			if (pVariable) {
		    			AddContenttoCharVariable(pVariable, pKey);
		    		}
		    	}
    		}

	    	if (pKey[strlen(pKey)-1] == '"')  {
			    AddMemberOnTop(&plVariable, pVariable);
		    	Nb++;
	    		openVariable = false;
	    	}
    	}
    }
  } while (pKey = strtok_r(NULL, " \n\r", &pSave));

	if (findCommand) {
		if(pCommand->nbArg == Nb) {
			pEntity = InitEntity(pCommand, pSaveCmd);
			if (pEntity)
				pEntity->plVariable = plVariable;
		}
		else
			printf("Wrong argument number !\n");
	}
	else {
		printf("Command not found !\n");
	}
	fflush(stdout);
	return pEntity;
}

int ExecEntity(CentralExec* pCentralExec, Entity* pEntity) {
	Command* pCommand;

	pCommand = pEntity->pCommand;
	if (!pCommand)
		return 0;

	if (pEntity->pOutput)
		FreeVariable(pEntity->pOutput);

	if (pCommand->nbArg == 0) {
    pEntity->pCommand->pFunction(pCentralExec, NULL);
  }
	else{
		pEntity->pCommand->pFunction(pCentralExec, pEntity);
	}

	pCentralExec->History++;
	pCentralExec->Current = -1;
	AddMemberOnTop(&pCentralExec->plHistory, pEntity);

	return 1;
}


Command* FindCommand(List* plDictionnary, char* pKey) {
  Command* pCommand;
  Chaine* pChaine;

  pChaine = plDictionnary->Main;
  while (pChaine) {
    pCommand = (Command*)pChaine->Member;
    if ((memcmp(pCommand->CName, pKey, strlen(pKey)) == 0) && (strlen(pKey) == strlen(pCommand->CName)))
    	return pCommand;

    pChaine = pChaine->Next;
  }

  return NULL;
}

int RunCentralExec(CentralExec* pCentralExec) {
  TaskManager* pTaskManager;

  pTaskManager = InitTaskManager();

  AddNewThread(pTaskManager, pCentralExec->pThread);
  StartingTaskManager(pTaskManager);
  WaitingTaskManager(pTaskManager);

  return 1;
}

