#include "bl.h"
#include "b64.h"

void printtext(unsigned char x[])
{
   int i;
   for (i=0; i < 16; i++)
      printf("%02x",x[i]);
   printf("\n");
}

int main() {
    uchar* iSecret = "DesireSecretKeyR";
    uchar* Clear = "MadenItr";
    uchar *StrCrypt;
    uchar *StrB64;

#ifdef _MEM_LEAKED_
    while(1) {
#endif

    StrCrypt = (uchar*)BFishCrypt(iSecret, Clear, strlen(Clear));
    printtext(StrCrypt);

    StrB64 = (uchar*)B64Encode((unsigned char*)StrCrypt, strlen(StrCrypt));
    printf("%d - %s\n", strlen(StrB64), StrB64);

    free(StrCrypt);
    StrCrypt = (uchar*)B64Decode((unsigned char*)StrB64, strlen(StrB64));
    printf("%d - %s\n", strlen(StrCrypt), StrCrypt);

    free(StrB64);
    BFishUncrypt(iSecret, StrCrypt, strlen(StrCrypt));
    printf("%d - %s\n", strlen(StrCrypt), StrCrypt);

    free(StrCrypt);

#ifdef _MEM_LEAKED_
    }
#endif

    return 0;
}

