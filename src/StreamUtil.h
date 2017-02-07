#ifndef STREAMUTIL_H
#define STREAMUTIL_H

#include <stdio.h>


int posnOfNull(unsigned char * str, int maxLen);
unsigned int fromLittleEndianStreamToInt(unsigned char * data);
unsigned int fromBigEndianStreamToInt(unsigned char * data);
int readBigEndianStreamToInt(FILE * data, unsigned int * result);
unsigned long long fromBigWordSmallWordBigEndianWords(unsigned char * data);
int iterateThroughStream(FILE* f, int dataLength);
int getId(FILE * f, int idSize, unsigned long long * result);

#endif