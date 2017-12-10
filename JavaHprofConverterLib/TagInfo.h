#ifndef TAG_INFO_H
#define TAG_INFO_H

#include <stdio.h>
#include "StreamReader.hpp"


/* Contains all the info necessary for tag processing */
struct TagInfo {
	FILE * stream;
	StreamReader * streamReader;
	long long dataLength;
	int idSize;
};
typedef struct TagInfo TagInfo;
TagInfo makeTagInfo(FILE * f, long long dataLength, int idSize);

#endif