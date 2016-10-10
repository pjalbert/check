#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "archetyp.h"

#ifndef __uuid_h__
#define __uuid_h__

#define N 624
#define M 397
#define MATRIX_A    0x9908b0dfUL    /* constant vector a */
#define UPPER_MASK  0x80000000UL    /* most significant w-r bits */
#define LOWER_MASK  0x7fffffffUL    /* least significant r bits */

#define KLen 1000

#define mix(a,b,c) { \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

typedef struct __uuid {
    ushort Version;
    uint* Mtrix;
    ulong High;
    ulong Low;
    uchar Id[17];
} Uuid;

uint* SeedGen();
uint* IArray(uint iKey[], uint iSeed);

uint   GenRandom_UInt(uint Mtrix[]);
ulong  GenRandom_ULong(uint Mtrix[]);
double GenRandom_UReal(uint Mtrix[]);

Uuid* SetUUID();
int GenerateUUID(Uuid*);
char* GetUUIDString(Uuid*);
int PrintUUID(Uuid*);
int FreeUUID(Uuid*);

#endif
