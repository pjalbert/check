#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef __Archetyp_h__

#define __Archetyp_h__

#define uchar   unsigned char        // 8-bit
#define ushort  unsigned short       // 16-bit
#define uint    unsigned int         // 32-bit
#define ulong   unsigned long long   // 64-bit

#define max(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define min(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define Is_BigEndian (!(union { ushort u16; uchar c; }){ .u16 = 1 }.c)

#define Checkpoint printf ("Line %d of file %s (function %s)\n", __LINE__, __FILE__, __func__)

#define ReduceArray(Array, Size)                                \
        ( {  int i, j;                                          \
            for (i = 0, j = 0; i < (Size); i++) {               \
                if (j) (Array)[i-j] = (Array)[i];               \
                if ((Array[i - j]) == 0) j++;                   \
            }                                                   \
            if (j) for (i = 0; i < j; i++)(Array)[(Size -1 -i)] = 0; })

#define SizeArray(Array, Size)                              \
        ( { int i;                                         \
            i = (Size);                                     \
            while((Array)[i-1] == 0)  i--;                   \
            i*1; } )


typedef enum {false,true} boolean;

#define Max128  128
#define Max256  256
#define Max512  512
#define Max1024 1024

#endif
