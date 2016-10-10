#include "message.h"

int main (int argn, char* argc[]) {
  Messages* pMessages;
  char* pSerial;
  char * iSecret = "03fab7d9cda9103a9c65ef891734cf34f9e43051e249c58d581f2128e7914efcdcdeac2bf722049c47a326f7b1eac230e83a5bea6a31482cc2b469ba924c5aa9";
	
	pMessages = (Messages*)InitMessage(iSecret);
	if (!pMessages) 
	{
		printf("Error Creating pMessage - \n");
		exit(-1);
	}	
  
  AddMessageContentKeyword(pMessages, "*Sending*Request*VarName-File://content:Sourcing-C:");
  AddMessageContentFile(pMessages, "blowfish.c");

	CryptMessage(pMessages, iSecret);

  pSerial = (char*)SerializeHeader(pMessages->pHeader);
  printf("pHeader-> %s\n", pSerial);
  printf("pMessage-> %d-%64.64s\n", pMessages->Length, pMessages->pContent);

	DecryptMessage(pMessages, iSecret);
  printf("pHeader-> %s\n", pSerial);
  printf("pMessage-> %d-%64.64s\n", pMessages->Length, pMessages->pContent);

  free(pSerial);
  
	return 0;
}