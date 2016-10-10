#include "uuid.h"

uint* SeedGen() {
    int i;
    uint a, b, c;
    uint MyKey[1000];
    struct timeval time;

    gettimeofday(&time,NULL);

    a = clock();
    b = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    c = getpid();
    mix (a, b, c);

    srand(c);

    for(i = 0; i <= 1000; i++) {
        MyKey[i] = rand();
    }

    return IArray(MyKey, (uint)rand());
}

uint* IArray(uint iKey[], uint iSeed) {
    int i, j, k;
    uint* Mtrix;
    uint mti;

    Mtrix = (uint*)malloc(sizeof(int)*(N+1));
    if(!Mtrix)
        return NULL;

    Mtrix[0]= iSeed & 0xffffffffUL;
    for (mti = 1; mti < N; mti++) {
        Mtrix[mti] = (1812433253UL * (Mtrix[mti-1] ^ (Mtrix[mti-1] >> 30)) + mti);
        Mtrix[mti] &= 0xffffffffUL;
    }

    i=1; j=0;
    k = (N > KLen ? N : KLen);
    for (; k; k--) {
        Mtrix[i] = (Mtrix[i] ^ ((Mtrix[i-1] ^ (Mtrix[i-1] >> 30)) * 1664525UL)) + iKey[j] + j; /* non linear */
        Mtrix[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i >= N) { Mtrix[0] = Mtrix[N-1]; i=1; }
        if (j >= KLen) j=0;
    }

    for (k = N-1; k; k--) {
        Mtrix[i] = (Mtrix[i] ^ ((Mtrix[i-1] ^ (Mtrix[i-1] >> 30)) * 1566083941UL)) - i; /* non linear */
        Mtrix[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i >= N) { Mtrix[0] = Mtrix[N-1]; i=1; }
    }

    Mtrix[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
    Mtrix[N] = mti;          /* We store Mtrix Index @ the end */

    return Mtrix;
}

uint GenRandom_UInt(uint Mtrix[]) {
    uint y;
    int kk;
    int mti;
    static uint mag01[2] = {0x0UL, MATRIX_A};

    mti = Mtrix[N];

    if (mti >= N) { /* generate N words at one time */
        for (kk = 0; kk < N - M; kk++) {
            y = (Mtrix[kk] & UPPER_MASK) | (Mtrix[kk + 1] & LOWER_MASK);
            Mtrix[kk] = Mtrix[kk + M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }

        for (; kk < N - 1; kk++) {
            y = (Mtrix[kk] & UPPER_MASK) | (Mtrix[kk + 1] & LOWER_MASK);
            Mtrix[kk] = Mtrix[kk + (M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }

        y = (Mtrix[N-1] & UPPER_MASK) | (Mtrix[0] & LOWER_MASK);
        Mtrix[N - 1] = Mtrix[M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }

    y = Mtrix[mti++];

    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    Mtrix[N] = mti;

    return y;
}

double GenRandom_UReal(uint Mtrix[]) {

    return GenRandom_UInt(Mtrix)*(1.0/(2^32 - 1));
}

ulong GenRandom_ULong(uint Mtrix[]) {

    return ((ulong)GenRandom_UInt(Mtrix) << 32) | (ulong)GenRandom_UInt(Mtrix);
}

Uuid* SetUUID() {
    Uuid* pUuid;

    pUuid = (Uuid*) malloc(sizeof(Uuid)*1);
    if (!pUuid)
        return NULL;

    // Version 1.0
    pUuid->Version = (ushort)((0x01 << 8) | 0x00);

    pUuid->Mtrix = NULL;
    pUuid->Mtrix = SeedGen();

    memset(pUuid->Id, 0, 17);

    return pUuid;
}

int GenerateUUID(Uuid* pUuid) {
    char *ptr;

    memset(pUuid->Id, 0, 17);
    pUuid->High = GenRandom_ULong(pUuid->Mtrix);
    ptr = (char*)&pUuid->High;
    memcpy(pUuid->Id, ptr, 8);

    pUuid->Low  = GenRandom_ULong(pUuid->Mtrix);
    ptr = (char*)&pUuid->Low;
    memcpy(pUuid->Id + 8, ptr, 8);

    // RFC 4122 Section 4.4
    pUuid->Id[6] = 0x40 | (pUuid->Id[6] & 0xf);
    pUuid->Id[8] = 0x80 | (pUuid->Id[8] & 0x3f);

    return 1;
}

int PrintUUID(Uuid* pUuid) {
    int i;

    for (i=0; i< 16; i++) {
        printf("%02x",pUuid->Id[i]);
        if ((i == 4) || (i == 6) || (i == 8) || (i == 10))
            printf("-");
    }
    printf("\n");

    return 1;
}

char* GetUUIDString(Uuid* pUuid) {
    int i, k;
    char Hex[3];
    char * Tmp;

    Tmp = (char*)malloc(sizeof(char)*37);
    if (!Tmp)
        return NULL;

    memset(Tmp, 0, 37);
    memset(Hex, 0, 3);

    for (i=0; i< 16; i++) {
        sprintf(Hex, "%02x",pUuid->Id[i]);
        strncat(Tmp, Hex, 2);
        if ((i == 4) || (i == 6) || (i == 8) || (i == 10))
            strncat(Tmp, "-", 1);
    }

    return Tmp;
}


int FreeUUID(Uuid* pUuid) {

    if (pUuid->Mtrix)
        free(pUuid->Mtrix);

    free(pUuid);

    return 1;
}
