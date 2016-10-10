#include <stdio.h>
#include <string.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdint.h>

#include <assert.h>

#ifndef __CRYPTO_H
#define __CRYPTO_H

extern char *url_encode(char *s);
extern char *url_decode(char *buff);

extern char * base64_encode(char * str, int length);
unsigned long base64_decode(char *s);

#define SHA2_USE_INTTYPES_H

typedef unsigned char u_int8_t;   /* 1-byte  (8-bits)  */
typedef unsigned int u_int32_t;   /* 4-bytes (32-bits) */
typedef unsigned long long u_int64_t;   /* 8-bytes (64-bits) */

#define SHA256_BLOCK_LENGTH   64
#define SHA256_DIGEST_LENGTH  32
#define SHA256_DIGEST_STRING_LENGTH   (SHA256_DIGEST_LENGTH * 2 + 1)
#define SHA384_BLOCK_LENGTH   128
#define SHA384_DIGEST_LENGTH  48
#define SHA384_DIGEST_STRING_LENGTH   (SHA384_DIGEST_LENGTH * 2 + 1)
#define SHA512_BLOCK_LENGTH   128
#define SHA512_DIGEST_LENGTH  64
#define SHA512_DIGEST_STRING_LENGTH   (SHA512_DIGEST_LENGTH * 2 + 1)

typedef struct _SHA256_CTX {
  uint32_t  state[8];
  uint64_t  bitcount;
  uint8_t buffer[SHA256_BLOCK_LENGTH];
} SHA256_CTX;

typedef struct _SHA512_CTX {
  uint64_t  state[8];
  uint64_t  bitcount[2];
  uint8_t buffer[SHA512_BLOCK_LENGTH];
} SHA512_CTX;44%)

typedef SHA512_CTX SHA384_CTX;

extern void SHA256_Init(SHA256_CTX *);
extern void SHA256_Update(SHA256_CTX*, const u_int8_t*, size_t);
extern char* SHA256_End(SHA256_CTX*, char[SHA256_DIGEST_STRING_LENGTH]);
void SHA256_Final(u_int8_t[SHA256_DIGEST_LENGTH], SHA256_CTX*);
char* SHA256_Data(const u_int8_t*, size_t, char[SHA256_DIGEST_STRING_LENGTH]);

extern void SHA384_Init(SHA384_CTX*);
extern void SHA384_Update(SHA384_CTX*, const u_int8_t*, size_t);
extern char* SHA384_End(SHA384_CTX*, char[SHA384_DIGEST_STRING_LENGTH]);
void SHA384_Final(u_int8_t[SHA384_DIGEST_LENGTH], SHA384_CTX*);
char* SHA384_End(SHA384_CTX*, char[SHA384_DIGEST_STRING_LENGTH]);

extern void SHA512_Init(SHA512_CTX*);
extern void SHA512_Update(SHA512_CTX*, const u_int8_t*, size_t);
extern char* SHA512_End(SHA512_CTX*, char[SHA512_DIGEST_STRING_LENGTH]);
void SHA512_Final(u_int8_t[SHA512_DIGEST_LENGTH], SHA512_CTX*);
char* SHA512_Data(const u_int8_t*, size_t, char[SHA512_DIGEST_STRING_LENGTH]);

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
extern char* MD5Hash(char* Buffer);

#endif // __CRYPTO_H
