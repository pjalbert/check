#include "uuid.h"

int main(void)
{
    int i;
    int Tab[6] = {1,0,1,1,1,1};
    int Tob[6] = {0,0,0,0,0,1};
    int Tib[6] = {0,1,0,1,0,0};
    uint* Mtrix;
    Uuid* pUuid;

    printf("%d / %d %d %d %d %d %d - ", SizeArray(Tab, 6), Tab[0], Tab[1], Tab[2], Tab[3], Tab[4], Tab[5]);
    ReduceArray(Tab, 6);
    printf("%d / %d %d %d %d %d %d\n", SizeArray(Tab, 6), Tab[0], Tab[1], Tab[2], Tab[3], Tab[4], Tab[5]);

    printf("%d / %d %d %d %d %d %d - ", SizeArray(Tob, 6), Tob[0], Tob[1], Tob[2], Tob[3], Tob[4], Tob[5]);
    ReduceArray(Tob, 6);
    printf("%d / %d %d %d %d %d %d\n", SizeArray(Tob, 6), Tob[0], Tob[1], Tob[2], Tob[3], Tob[4], Tob[5]);

    printf("%d %d %d %d %d %d - ", Tib[0], Tib[1], Tib[2], Tib[3], Tib[4], Tib[5]);
    ReduceArray(Tib, 6);
    printf("%d %d %d %d %d %d\n", Tib[0], Tib[1], Tib[2], Tib[3], Tib[4], Tib[5]);

    Mtrix = SeedGen();

    printf("10 outputs of GenRandom_UInt()\n");

    for (i=0; i<10; i++) {
      printf("%10lu ", GenRandom_UInt(Mtrix));
      if (i%5==4) printf("\n");
    }

    printf("10 outputs of GenRandom_UInt() % 256\n");
    for (i=0; i<20; i++) {
      printf("%4d", (ushort)GenRandom_UInt(Mtrix)%256);
      if (i%10==4) printf("\n");
    }

    printf("\n10 outputs of GenRandom_ULong()\n");

    for (i=0; i<10; i++) {
      printf("%20llu ", GenRandom_ULong(Mtrix));
      if (i%5==4) printf("\n");
    }

    printf("\n10 outputs of GenRandom_UReal()\n");
    for (i=0; i<10; i++) {
      printf("%10.8f ", GenRandom_UReal(Mtrix));
      if (i%5==4) printf("\n");
    }

    free(Mtrix);

    pUuid = SetUUID();

    printf("\n10 outputs of GenerateUUID()\n");
    for (i=0; i<10; i++) {
        GenerateUUID(pUuid);
        PrintUUID(pUuid);
    }

    FreeUUID(pUuid);

    return 0;
}
