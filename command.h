#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/utsname.h>
#include <time.h>

#include <unistd.h>
#include <sys/select.h>
#include <termios.h>

#include <arpa/inet.h>
#include <ifaddrs.h>

#include "md5.h"
#include "list.h"
#include "thread.h"

#ifndef __command_h__
#define __command_h__

#define KEY_LEFT  	0x25
#define KEY_UP  		0x26
#define KEY_RIGHT  	0x27
#define KEY_DOWN  	0x28
#define KEY_ESCAPE  0x1B

typedef int (ExecComm)(void *, void *);

#define AIP_FILE 10100
#define AIP_CHAR 10200
#define AIP_BLOB 10300
#define AIP_FCTN 20100
#define AIP_HASH 20200

/**
Archetyp is Metadata#Content
MetaData is :Type:Some Information related to type:
Example
	:file:/var/log/php.log:#"Content"
	:char:#A varchar with some undefine length
	:pict:jpeg:/picture/MyJump.jpg:# -
	:hash:md5:#acd6549acd...96f4d  */
typedef struct __Variable {
	short Type;
	unsigned long Length;
	unsigned char* Content;
} Variable;

typedef struct __Command {
	char* Category;
	char* CName;
	char* Xplain;
	boolean Simple;
	boolean Block;
  ExecComm* pFunction;
  unsigned short nbArg;
} Command;

typedef struct __Entity {
	char* CmdLine;
	Command* pCommand;
	List* plVariable;
	List* plEntities;
	Variable* pOutput;
} Entity;

typedef struct __CentralExec {
	char* Prompt;
	struct termios Term_OS;
	boolean Continue;
	List* plDictionnary;
	unsigned long Current;
	unsigned long History;
	List* plHistory;
	Thread* pThread;
} CentralExec;

Command* InitCommand(char* , ExecComm* , char*, char*, unsigned short , boolean );
Command* FindCommand(List* , char* );

Entity* InitEntity(Command* , char* );

int Cmd_Net(void* , void*);
int Cmd_Host(void* , void*);
int Cmd_Date(void* , void*);
int Cmd_Time(void* , void*);
int Cmd_Help(void* , void*);
int Cmd_Quit(void* , void*);

int Cmd_Md5(void* , void*);

CentralExec* InitCentralExec(char* );
int CentralExecParse(CentralExec* , char* );
void FreeCentralExec(void* );

Entity* CreateEntity(CentralExec* , char* );

Variable* CreateVariable(char* );
Variable* SetAutomateVariable(int , char* , char* );
int AddContenttoCharVariable(Variable* , char*);
int FreeVariable(Variable *);
unsigned char* GiveContentFromVariable(Variable* );

Entity* callHistory(CentralExec* , boolean );
int ExecEntity(CentralExec* , Entity* );

int DefaultCommandStarting(Thread* );
int DefaultCommandRunning(Thread* );
int DefaultCommandEnding(Thread* );

#endif
