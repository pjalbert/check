#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "archetyp.h"


#ifndef __BL_H__
#define __BL_H__

#define F(x,t) t = keystruct->s[0][(x) >> 24]; \
               t += keystruct->s[1][((x) >> 16) & 0xff]; \
               t ^= keystruct->s[2][((x) >> 8) & 0xff]; \
               t += keystruct->s[3][(x) & 0xff];
#define swap(r,l,t) t = l; l = r; r = t;
#define ITERATION(l,r,t,pval) l ^= keystruct->p[pval]; F(l,t); r^= t; swap(r,l,t);

typedef struct {
   uint p[18];
   uint s[4][256];
} BLOWFISH_KEY;


unsigned char* BFishCrypt(unsigned char*, unsigned char*, unsigned long);
unsigned long BFishUncrypt(unsigned char*, unsigned char*, unsigned long);


#endif
