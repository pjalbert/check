#include "json.h"

void Unpad(unsigned char* cTemp, char c) {
    uint i,j;

    i = 0;
    j = strlen((char*)cTemp);

    while (cTemp[i] == c) i++;
    while (cTemp[j-1] == c) j--;

    if (j != strlen(cTemp)) {
        if (i != 0)
            memcpy(cTemp, cTemp + i, j - i);
        memset(cTemp + j-i, 0, strlen(cTemp) - j);
    } else {
        if (i != 0)
            memcpy(cTemp, cTemp + i, j - i);
        memset(cTemp + j-i, 0, strlen(cTemp) - j + i);
    }


}

JsonParser* InitJsonParser(char* pJsonStr, int Level,boolean Store) {
	JsonParser* pJsonParser;
	JsonToken* pJsonToken;
	ulong Lng, i, j, k;
	ulong openBrace, closeBrace;
	ulong openArray, closeArray;
	ulong rangeComma, rangeColon;
	char* pTmp, *pStr;
	boolean notEmpty;
	boolean notQuote;


	pJsonParser = (JsonParser*)malloc(sizeof(JsonParser));
	if (!pJsonParser)
        return NULL;

    pJsonParser->NbElement = 0;
    pJsonParser->plJsonToken = NULL;

    Lng = strlen(pJsonStr);

    if ((pJsonStr[0] != '{') || (pJsonStr[Lng-1] != '}'))
        if ((pJsonStr[0] != '[') || (pJsonStr[Lng-1] != ']'))
            return NULL;

    i = 1;
    openBrace = closeBrace = 0;
    openArray = closeArray = 0;
    rangeComma = rangeColon = 0;
    notEmpty = false;
    notQuote = true;
    pJsonToken = NULL;
    pStr = NULL;

    while(i < Lng - 1) {

        /* Backslash escapement */
        if (pJsonStr[i] == '\\') {
            if (i + 1 < Lng - 1)
            switch (pJsonStr[++i]) {
                case '\"': case '/' : case '\\' : case 'b' :
                case 'f' : case 'r' : case 'n'  : case 't' :
                    break;
                case 'u' :
                    break;
                default:
                    return NULL;
            }
        }

        if ((pJsonStr[i] != ' ') && (pJsonStr[i] != '}') && (pJsonStr[i] != ']')
            && (pJsonStr[i] != '{') && (pJsonStr[i] != '['))
            notEmpty = true;

        if (pJsonStr[i] == '\"') {
            if (!notQuote)
                notQuote = true;
            else
                notQuote = false;
        }

        j = i;
        if ((pJsonStr[i] == '{')){
            openBrace = i;
            closeBrace = 0;
            k = 0;
            while ((j++ < Lng) && (closeBrace == 0)) {

                if ((pJsonStr[j] == '}') && (k == 0))
                    closeBrace = j;
                else
                    if (pJsonStr[j] == '}')
                        k--;
                if (pJsonStr[j] == '{')
                    k++;
            }

            if (!closeBrace)
                return NULL;

            if (closeBrace > openBrace) {
                pStr = (char*)malloc(sizeof(char)*(closeBrace - openBrace) + 2);
                memset(pStr, 0, (closeBrace - openBrace) + 2);
                memcpy(pStr, pJsonStr + openBrace, closeBrace - openBrace + 1);

                if (!pJsonToken)
                    pJsonToken = InitJsonToken(JSN_OBJECT);
                else
                    pJsonToken->Type = JSN_OBJECT;
                pJsonToken->pJsonParser = InitJsonParser(pStr, Level+1, Store);
                if (!pJsonToken->pJsonParser)
                    return NULL;
                pJsonToken->Complete = true;
                i = closeBrace+1;
                notEmpty = false;
            }
        }

        if ((pJsonStr[i] == '[') ) {
            openArray = i;
            closeArray = 0;

            k = 0;
            while ((j++ < Lng) && (closeArray == 0)) {

                if ((pJsonStr[j] == ']') && (k == 0))
                    closeArray = j;
                else
                    if (pJsonStr[j] == ']')
                        k--;
                if (pJsonStr[j] == '[')
                    k++;
            }

            if (!closeArray)
                return NULL;

            if (closeArray > openArray) {
                pStr = (char*)malloc(sizeof(char)*(closeArray - openArray) + 2);
                memset(pStr, 0, (closeArray - openArray) + 2);
                memcpy(pStr, pJsonStr + openArray, closeArray - openArray + 1);

                if (!pJsonToken)
                    pJsonToken = InitJsonToken(JSN_ARRAY);
                else
                    pJsonToken->Type = JSN_ARRAY;
                pJsonToken->pJsonParser = InitJsonParser(pStr, Level+1, Store);
                if (!pJsonToken->pJsonParser)
                    return NULL;
                pJsonToken->Complete = true;
                i = closeArray+1;
                notEmpty = false;
            }
        }

        if ((pJsonStr[i] == ':') && (notQuote)) {
            pJsonToken = InitJsonToken(JSN_STRING);

            if (rangeComma) {
                pStr = (char*)malloc(sizeof(char)*(i-rangeComma));
                memset(pStr, 0, (i-rangeComma));
                memcpy(pStr, pJsonStr + rangeComma + 1, (i-rangeComma)-1);
            }
            else {
                pStr = (char*)malloc(sizeof(char)*i + 1);
                memset(pStr, 0, i + 1);
                memcpy(pStr, pJsonStr, i);
            }

            rangeColon = i;

            pJsonToken->pKey = (char*)malloc(sizeof(char)*strlen(pStr) + 1 );
            memset(pJsonToken->pKey, 0, sizeof(char)*strlen(pStr) + 1);
            memcpy(pJsonToken->pKey, pStr + 1, strlen(pStr));
            Unpad(pJsonToken->pKey, ' ');

        }

        if ((pJsonStr[i] == ',') && (notQuote)) {
            if (!pJsonToken)
                pJsonToken = InitJsonToken(JSN_STRING);

            if (pStr)
                free(pStr);
            pStr = NULL;

            if (rangeComma) {
                if (rangeColon > rangeComma) {
                    pStr = (char*)malloc(sizeof(char)*(i-rangeColon));
                    memset(pStr, 0, (i-rangeColon) );
                    memcpy(pStr, pJsonStr + rangeColon + 1, (i-rangeColon) - 1);
                }
                else {
                    pStr = (char*)malloc(sizeof(char)*(i-rangeComma) );
                    memset(pStr, 0, (i-rangeComma) );
                    memcpy(pStr, pJsonStr + rangeComma + 1, (i-rangeComma) -1 );
                }
            }
            else {
                if (rangeColon) {
                    pStr = (char*)malloc(sizeof(char)*(i-rangeColon));
                    memset(pStr, 0, (i-rangeColon));
                    memcpy(pStr, pJsonStr + rangeColon + 1, (i-rangeColon) - 1);
                }
                else {
                    pStr = (char*)malloc(sizeof(char)*i);
                    memset(pStr, 0, i);
                    memcpy(pStr, pJsonStr + 1, i - 1);
                }
            }

            rangeComma = i;
            pJsonToken->pValue = (char*)malloc(sizeof(char)*strlen(pStr) + 1);
            memset(pJsonToken->pValue, 0, sizeof(char)*strlen(pStr) + 1);
            memcpy(pJsonToken->pValue, pStr, strlen(pStr));
            pJsonToken->Complete = true;
            Unpad(pJsonToken->pValue, ' ');
        }

        if ((pJsonToken) && (pJsonToken->Complete)) {
            pJsonParser->NbElement++;
            pJsonToken->Level = Level;

            AddMemberOnBottom(&pJsonParser->plJsonToken, pJsonToken);
            pJsonToken = NULL;
            notEmpty = false;
            if (pStr)
                free(pStr);
            pStr = NULL;

        }

        i++;
    }

    if (notEmpty) {
        if (!pJsonToken)
            pJsonToken = InitJsonToken(JSN_STRING);

        if (pStr)
            free(pStr);
        pStr = NULL;

        if (rangeComma) {
            if (rangeColon > rangeComma) {
                pStr = (char*)malloc(sizeof(char)*(Lng-rangeColon) - 1);
                memset(pStr, 0, (Lng-rangeColon) - 1);
                memcpy(pStr, pJsonStr + rangeColon + 1, (Lng-rangeColon) - 2);
            }
            else {
                pStr = (char*)malloc(sizeof(char)*(Lng-rangeComma) - 1);
                memset(pStr, 0, (Lng-rangeComma) - 1);
                memcpy(pStr, pJsonStr + rangeComma + 1, (Lng-rangeComma) - 2);
            }
        }
        else {
            if (rangeColon) {
                pStr = (char*)malloc(sizeof(char)*(Lng-rangeColon) - 1);
                memset(pStr, 0, (Lng-rangeColon) - 1);
                memcpy(pStr, pJsonStr + rangeColon + 1, (Lng-rangeColon) - 2);
            }
            else {
                pStr = (char*)malloc(sizeof(char)*Lng - 1);
                memset(pStr, 0, Lng - 1);
                memcpy(pStr, pJsonStr + 1, Lng - 2);
            }
        }

        pJsonToken->pValue = (char*)malloc(sizeof(char)*strlen(pStr) + 1);
        memset(pJsonToken->pValue, 0, sizeof(char)*strlen(pStr) + 1);
        memcpy(pJsonToken->pValue, pStr, strlen(pStr));
        Unpad(pJsonToken->pValue, ' ');

        pJsonParser->NbElement++;
        pJsonToken->Level = Level;
        AddMemberOnBottom(&pJsonParser->plJsonToken, pJsonToken);
        pJsonToken = NULL;
    }


	return pJsonParser;
}

JsonToken* InitJsonToken(JSType Type) {
	JsonToken* pJsonToken;

	pJsonToken = (JsonToken*)malloc(sizeof(JsonToken));
	if (!pJsonToken)
        return 0;

    pJsonToken->Type = Type;
    pJsonToken->Complete = false;

    pJsonToken->pKey = NULL;
    pJsonToken->pValue = NULL;
    pJsonToken->pJsonParser = NULL;


	return pJsonToken;
}

int DumpJsonStruct(JsonParser* pJsonParser) {
    JsonToken *pJsonToken;
    Chaine* pChaine;
    int i;

    pChaine = pJsonParser->plJsonToken->Main;

    while (pChaine) {
        pJsonToken = (JsonToken*)pChaine->Member;

        for(i = 0; i < pJsonToken->Level; i++)
            printf("\t");

        printf("pJSonToken -> %d ", pJsonToken->Type);
        if (pJsonToken->pKey)
            printf("- %s ", pJsonToken->pKey);

        switch(pJsonToken->Type) {
            case JSN_OBJECT: case JSN_ARRAY:
                printf("\n");
                DumpJsonStruct(pJsonToken->pJsonParser);
                break;
            default:
                if (pJsonToken->pValue)
                    printf("- %s \n", pJsonToken->pValue);
                break;
        }

        pChaine = pChaine->Next;
    }

    return 1;
}

unsigned char *GetJsonElementValue(JsonParser* pJsonParser, unsigned char* Search) {
    JsonToken *pJsonToken;
    Chaine* pChaine;
    uchar* cTmp, *pMem;
    int len;

    pChaine = pJsonParser->plJsonToken->Main;

    len = 0;
    while ((len < strlen(Search)) && (Search[len] != '.')) len++;

    while (pChaine) {
        pJsonToken = (JsonToken*)pChaine->Member;

        if (pJsonToken->pKey) {
            if ((len == strlen(pJsonToken->pKey) - 2)
                && (memcmp(pJsonToken->pKey+1, Search, len) == 0)) {
                if ((pJsonToken->Type != JSN_OBJECT) && (pJsonToken->Type != JSN_ARRAY)) {
                    return pJsonToken->pValue;
                }
                else {

                    if (len != strlen(Search)) {
                        cTmp = (unsigned char *)malloc(sizeof(char)*(strlen(Search) - len) + 1);
                        memset (cTmp, 0, strlen(Search) - len + 1);
                        memcpy(cTmp, Search + len + 1, strlen(Search) - len );
                        pMem = GetJsonElementValue(pJsonToken->pJsonParser, cTmp);
                        free(cTmp);
                        if (pMem) return pMem;
                    }
                }
            }
        }
        else {
            if (((pJsonToken->Type == JSN_OBJECT) || (pJsonToken->Type == JSN_ARRAY))) {
                    pMem = GetJsonElementValue(pJsonToken->pJsonParser, Search);
                    if (pMem) return pMem;
            }
        }

        pChaine = pChaine->Next;
    }

    return NULL;
}

int GetJsonElementCount(JsonParser* pJsonParser, unsigned char* Search) {
    JsonToken *pJsonToken;
    Chaine* pChaine;
    uchar* cTmp, *pMem;
    int Nb, len;

    pChaine = pJsonParser->plJsonToken->Main;

    len = 0;
    Nb = 0;
    while ((len < strlen(Search)) && (Search[len] != '.')) len++;

    while (pChaine) {
        pJsonToken = (JsonToken*)pChaine->Member;

        if (pJsonToken->pKey) {
            if ((len == strlen(pJsonToken->pKey) - 2)
                && (memcmp(pJsonToken->pKey+1, Search, len) == 0)) {
                if ((pJsonToken->Type != JSN_OBJECT) && (pJsonToken->Type != JSN_ARRAY)) {
                    Nb++;
                }
                else {

                    if (len != strlen(Search)) {
                        cTmp = (unsigned char *)malloc(sizeof(char)*(strlen(Search) - len) + 1);
                        memset (cTmp, 0, strlen(Search) - len + 1);
                        memcpy(cTmp, Search + len + 1, strlen(Search) - len );
                        Nb +=  GetJsonElementCount(pJsonToken->pJsonParser, cTmp);
                        free(cTmp);
                    }
                }
            }
        }
        else {
            if (((pJsonToken->Type == JSN_OBJECT) || (pJsonToken->Type == JSN_ARRAY))) {
                    Nb += GetJsonElementCount(pJsonToken->pJsonParser, Search);
            }
        }

        pChaine = pChaine->Next;
    }

    return Nb;
}

int GetJsonElementList(JsonParser* pJsonParser, List** pList, unsigned char* Search) {
    JsonToken *pJsonToken;
    Chaine* pChaine;
    uchar* cTmp;
    int len;

    pChaine = pJsonParser->plJsonToken->Main;

    len = 0;
    while ((len < strlen(Search)) && (Search[len] != '.')) len++;

    while (pChaine) {
        pJsonToken = (JsonToken*)pChaine->Member;

        if (pJsonToken->pKey) {
            if ((len == strlen(pJsonToken->pKey) - 2)
                && (memcmp(pJsonToken->pKey+1, Search, len) == 0)) {
                if ((pJsonToken->Type != JSN_OBJECT) && (pJsonToken->Type != JSN_ARRAY)) {
                    AddMemberOnBottom(pList, pJsonToken->pValue);
                }
                else {

                    if (len != strlen(Search)) {
                        cTmp = (unsigned char *)malloc(sizeof(char)*(strlen(Search) - len) + 1);
                        memset (cTmp, 0, strlen(Search) - len + 1);
                        memcpy(cTmp, Search + len + 1, strlen(Search) - len );
                        GetJsonElementList(pJsonToken->pJsonParser, pList, cTmp);
                        free(cTmp);
                    }
                }
            }
        }
        else {
            if (((pJsonToken->Type == JSN_OBJECT) || (pJsonToken->Type == JSN_ARRAY))) {
                GetJsonElementList(pJsonToken->pJsonParser, pList, Search);
            }
        }

        pChaine = pChaine->Next;
    }

    return 1;
}


unsigned char *GetJsonElementValueAt(JsonParser* pJsonParser, unsigned char* Search, int indice) {
    int i;
    List* pList;
    Chaine* pChaine;
    unsigned char* pMem;

    pList = NULL;
    GetJsonElementList(pJsonParser, &pList, Search);
    if (!pList)
        return NULL;

    pChaine = pList->Main;
    for (i = 0; i < indice; i++)
        if (pChaine)
            pChaine = pChaine->Next;

    pMem = (unsigned char*)pChaine->Member;
    CleanList(pList);

    return pMem;
}
