#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifndef __B64_h__
#define __B64_h__

int Base64encode_len(int len);
int Base64encode(char * coded_dst, const char *plain_src,int len_plain_src);
unsigned char* B64Encode(char * coded_dst, int len_plain_src);

int Base64decode_len(const char * coded_src);
int Base64decode(char * plain_dst, const char *coded_src);
unsigned char* B64Decode(char * coded_dst, int len_plain_src);

#endif
