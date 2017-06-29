#ifndef TAG_INFO_H
#define TAG_INFO_H

#include <stdio.h>


/* Contains all the info necessary for tag processing */
struct TagInfo {
	FILE * stream;
	long long dataLength;
	int idSize;
};
typedef struct TagInfo TagInfo;
TagInfo makeTagInfo(FILE * f, long long dataLength, int idSize);

#endif