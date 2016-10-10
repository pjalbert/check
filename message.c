/*
 *  message.c
 *  Idiome
 *
 *  Created by Pierre-Jean ALBERT on Wed Apr 01 2014.
 *  Copyright (c) 2013 __An Intellectual Projet__. All rights reserved.
 * 
 */
#include "message.h"

/**
	KeyWord
		CNX - Init Connexion
												ACK - Confirm Connexion
		SND - Send Information
												RPY - Reply
		END - Asking for Closing Communication
												BYE - Confirm End of Communcication
*/
Header* InitHeader(char* iSecret) {
	Header* pHeader;
  char cSequency[9];
  
	pHeader = (Header*) malloc(sizeof(Header));
	if(!pHeader)
		return NULL;

	memset(pHeader->Key, 0, 3*sizeof(char));
	strcpy(pHeader->Key, "CNX");

	srand(time(NULL));
	pHeader->Sequency = rand()%256;

	pHeader->Sequency <<= 8;
	pHeader->Sequency += rand()%256;

	pHeader->Sequency <<= 8;
	pHeader->Sequency += rand()%256;

	pHeader->Sequency <<= 8;
	pHeader->Sequency += rand()%256;
	
	memset(pHeader->HashCode, 0, 32*sizeof(char));

	pHeader->Version = 0%256;

	pHeader->Version <<= 8;
	pHeader->Version += 1%256;
	
	pHeader->EoH = '\0';
	
	return pHeader;
}

char* SerializeHeader(Header* pHeader) {
	char* pSerial;
	
	pSerial = (char *)malloc(63*sizeof(char) + 1);
	if (!pSerial)
		return NULL;
	
	memset(pSerial, 0, 64);

	memcpy(pSerial, "CNX-", 4);

	sprintf(pSerial+4, "%08lx-", (unsigned long)pHeader->Sequency & 0xFFFFFFFFUL);
	sprintf(pSerial+13, "%32s-", pHeader->HashCode);
	sprintf(pSerial+47, "%04hx", (unsigned short)pHeader->Version & 0xFFFFUL);
	
	return pSerial;
}

int GenNewHeader(Header* pHeader) {
  char cSequency[33];

	pHeader->Sequency = rand()%256;

	pHeader->Sequency <<= 8;
	pHeader->Sequency += rand()%256;

	pHeader->Sequency <<= 8;
	pHeader->Sequency += rand()%256;

	pHeader->Sequency <<= 8;
	pHeader->Sequency += rand()%256;

	memset(cSequency, 0, 33);
	memcpy(cSequency, pHeader->HashCode, 32);
	MD5HashSequency(cSequency, pHeader->HashCode, pHeader->Sequency);
	
	return 1;
}

int AckHeader(Header* pHeader, char* iSecret) {
	char* pHash;
  char cSequency[9];
	
	pHash = (char *)malloc(32*sizeof(char));
	if (!pHash)
		return 0;
	if (iSecret) {
		memset(pHash, 0, 32*sizeof(char));
		MD5HashSequency(iSecret, pHash, pHeader->Sequency);
	}
	else {
		memset(cSequency, 0, 9);
		sprintf(cSequency, "%08lx", (unsigned long)pHeader->Sequency & 0xFFFFFFFFUL);
		memset(pHash, 0, 32*sizeof(char));
		MD5HashSequency(cSequency, pHash, pHeader->Sequency);
	}
	
	if (memcmp(pHash, pHeader->HashCode, 32) == 0)
		return 1;
	else
		return 0;
}
	
int FreeHeader(Header* pHeader) {
	
	return 1;
}


Messages* InitMessage(char* iSecret) {
	Messages* pMessages;
	
	pMessages = (Messages*)malloc(sizeof(Messages));
	if (!pMessages)
		return NULL;
		
	pMessages->pHeader = (Header *)InitHeader(iSecret);
	if (!pMessages->pHeader)
		return NULL;
	
	return pMessages;
}

int AddMessageContentKeyword(Messages* pMessages, char* KeyWord) {
	char *pContent;
	
	if (pMessages-> pContent) {
		pContent = realloc(pMessages-> pContent, pMessages->Length + strlen(KeyWord));
		if (!pContent)
			return 0;
		memcpy(pContent + pMessages->Length, KeyWord, strlen(KeyWord));
		pMessages-> pContent = pContent;
		pMessages->Length = pMessages->Length + strlen(KeyWord);
	}
	else {
		pContent = (char*)malloc(strlen(KeyWord)*sizeof(char));
		memset(pContent, 0, strlen(pContent));
		memcpy(pContent, KeyWord, strlen(KeyWord));
		pMessages-> pContent = pContent;
		pMessages->Length = strlen(KeyWord);
	}
	
	return 1;
}


int AddMessageContentFile(Messages* pMessages, char* FileName) {
  FILE *pFile;
	unsigned char *pContent;
  unsigned long Length;
  char* pTmp;
  unsigned long iReader;

  pFile=fopen(FileName,"rb");
  if (pFile == NULL)
  	return 0;

	fseek(pFile, 0, SEEK_END);
	Length = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	if (pMessages-> pContent) {
		pContent = (unsigned char*) realloc(pMessages-> pContent, pMessages->Length + Length);
		if (!pContent)
			return 0;
		pTmp = pContent + pMessages->Length;
	}
	else {
		pContent = (unsigned char*)malloc(Length*sizeof(char));
		if (!pContent)
			return 0;
		pTmp = pContent;
	}
	
	while (iReader = fread(pTmp, sizeof(char), 65536, pFile))	
		pTmp = pTmp + iReader;

	pMessages-> pContent = pContent;
	pMessages->Length = pMessages->Length + Length;
	
	fclose (pFile);
	return 1;
}

char* SerializeMessages(Messages* pMessages) {
	char* pSerial;


	return pSerial;
}

int CryptMessage(Messages* pMessages, char* iSecret) {
	Header* pHeader;
  BLOWFISH_CTX ctx;
	unsigned char* pContent;
	unsigned char _Buf[9];
	unsigned long EncBegin, EncEnd;
	int i;

	Blowfish_Init (&ctx, iSecret, strlen(iSecret));
  
  /** Padding pContent with 8-bloc 0x0000000000 */
  pMessages->Length = 8*(1 + (unsigned long)(pMessages->Length/8));
  pContent = (unsigned char*) malloc (pMessages->Length*sizeof(char) + 1);
  memset(pContent, 0, pMessages->Length*sizeof(char) + 1);

	memcpy(pContent, pMessages->pContent, pMessages->Length);

  for(i = 0; i < pMessages->Length; i += 8) {
  	EncBegin = (unsigned long)(pContent[i] | (pContent[i+1] << 8) | (pContent[i+2] << 16) | (pContent[i+3] << 24));
  	EncEnd = (unsigned long)(pContent[i+4] | (pContent[i+5] << 8) | (pContent[i+6] << 16) | (pContent[i+7] << 24));
    
    Blowfish_Encrypt(&ctx, &EncBegin, &EncEnd);

		memset(_Buf, 0, 9);
    sprintf(_Buf, "%c%c%c%c%c%c%c%c", 
    	EncBegin & 0xFF, (EncBegin >> 8) & 0xFF, (EncBegin >> 16) & 0xFF, (EncBegin >> 24) & 0xFF, 
    	EncEnd & 0xFF, (EncEnd >> 8) & 0xFF, (EncEnd >> 16) & 0xFF, (EncEnd >> 24) & 0xFF);
    memcpy(pContent + i, _Buf, 8);
  }
  
  free(pMessages->pContent);
  pMessages->pContent = pContent;

	pHeader = (Header*)pMessages->pHeader;
	memset(pHeader->HashCode, 0, 32*sizeof(char));
	MD5HashSequency(pMessages->pContent, pHeader->HashCode, pHeader->Sequency);

	return 1;
}

int DecryptMessage(Messages* pMessages, char* iSecret) {
	Header* pHeader;
	char pHash[33];
  BLOWFISH_CTX ctx;
	unsigned char* pContent;
	unsigned char _Buf[9];
	unsigned long EncBegin, EncEnd;
	int i;

	pHeader = (Header*)pMessages->pHeader;
	memset(pHash, 0, 33*sizeof(char));
	MD5HashSequency(pMessages->pContent, pHash, pHeader->Sequency);
	
	if (memcmp(pHeader->HashCode, pHash, 32) != 0)
		return 0;

	Blowfish_Init (&ctx, iSecret, strlen(iSecret));
  
  pContent = (unsigned char*) malloc (pMessages->Length*sizeof(char) + 1);
  memset(pContent, 0, pMessages->Length*sizeof(char) + 1);
  memcpy(pContent, pMessages->pContent, pMessages->Length);

  
  for(i = 0; i < pMessages->Length; i += 8) {
  	EncBegin = (unsigned long)(pContent[i] | (pContent[i+1] << 8) | (pContent[i+2] << 16) | (pContent[i+3] << 24));
  	EncEnd = (unsigned long)(pContent[i+4] | (pContent[i+5] << 8) | (pContent[i+6] << 16) | (pContent[i+7] << 24));
    
    Blowfish_Decrypt(&ctx, &EncBegin, &EncEnd);

		memset(_Buf, 0, 9);
    sprintf(_Buf, "%c%c%c%c%c%c%c%c", 
    	EncBegin & 0xFF, (EncBegin >> 8) & 0xFF, (EncBegin >> 16) & 0xFF, (EncBegin >> 24) & 0xFF, 
    	EncEnd & 0xFF, (EncEnd >> 8) & 0xFF, (EncEnd >> 16) & 0xFF, (EncEnd >> 24) & 0xFF);
    memcpy(pContent + i, _Buf, 8);
  }
  
  free(pMessages->pContent);
  pMessages->Length = strlen(pContent);
  pMessages->pContent = pContent;

	return 1;
}



