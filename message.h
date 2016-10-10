#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "list.h"
#include "md5.h"
#include "blowfish.h"

#ifndef __Message_h__
#define __Message_h__

typedef struct __Header {
	char Key[3];
  unsigned long Sequency;
  char HashCode[32];
  unsigned short Version;
  char EoH;
} Header;

typedef struct __Messages {
  Header* pHeader;
  unsigned long Length;
  unsigned char* pContent;
  char EoM;
} Messages;

Header* InitHeader(char* );
int GenNewHeader(Header* );
char* SerializeMessages(Messages* );

Messages* InitMessage(char* );

int AddMessageContentKeyword(Messages* , char* );
int AddMessageContentFile(Messages* , char* );
int AddMessageContentVarchar(Messages* , char* );
int	AddMessageContentLong(Messages* , long );

int CryptMessage(Messages* , char* );


#endif
