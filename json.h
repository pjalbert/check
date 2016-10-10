#ifndef __AIP_Json_h__
#define __AIP_Json_h

#include "archetyp.h"
#include "list.h"


typedef enum {JSN_PRIMITIVE = 0, JSN_OBJECT = 1, JSN_ARRAY = 2, JSN_STRING = 3, JSN_INTEGER = 4} JSType;


typedef struct __JsonParser {
    uint NbElement;
    unsigned char* Str;

	List* plJsonToken;
} JsonParser;

typedef struct __JsonToken {
	JSType Type;

	ushort Level;

	boolean Complete;

	uchar* pKey;
	uchar* pValue;

	JsonParser*  pJsonParser;

} JsonToken;


JsonParser* InitJsonParser(char* , int , boolean );
int DumpJsonStruct(JsonParser* );
JsonToken* InitJsonToken(JSType);
void Unpad(unsigned char* , char );

unsigned char* GetJsonElementValue(JsonParser* , unsigned char*);
int GetJsonElementCount(JsonParser* , unsigned char* );

int GetJsonElementList(JsonParser* , List** , unsigned char* );
unsigned char *GetJsonElementValueAt(JsonParser* , unsigned char* , int ) ;


#endif
