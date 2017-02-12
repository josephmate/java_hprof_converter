#ifndef STREAMUTIL_H
#define STREAMUTIL_H

#include <stdio.h>


int posnOfNull(unsigned char * str, int maxLen);
unsigned int fromTwoByteBigEndianStreamToInt(unsigned char * data);
unsigned int fromLittleEndianStreamToInt(unsigned char * data);
unsigned int fromBigEndianStreamToInt(unsigned char * data);
unsigned long long fromBigWordSmallWordBigEndianWords(unsigned char * data);
int readTwoByteBigEndianStreamToInt(FILE * f, unsigned int * result);
int readBigEndianStreamToInt(FILE * data, unsigned int * result);
int readBigWordSmallWordBigEndianStreamToLong(FILE * f, unsigned long long * result);
int iterateThroughStream(FILE* f, int dataLength);
int getId(FILE * f, int idSize, unsigned long long * result);

#endif