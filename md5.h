#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>

#ifndef __Md5_h__
#define __Md5_h__

typedef unsigned long int UINT4;

typedef struct {
  UINT4 i[2];   /* number of _bits_ handled mod 2^64 */
  UINT4 buf[4];  /* scratch buffer */
  unsigned char in[64];  /* input buffer */
  unsigned char digest[16];   /* actual digest after MD5Final call */
} MD5_CTX;

void MD5Init (MD5_CTX*);
void MD5Update (MD5_CTX*, unsigned char *, unsigned int);
void MD5Final (MD5_CTX* );
extern int MD5Hash(char* , char *);
extern int MD5HashSequency(char* , char* , unsigned long );

#endif 
