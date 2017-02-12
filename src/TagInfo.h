#ifndef TAG_INFO_H
#define TAG_INFO_H

#include <stdio.h>


/* Contains all the info necessary for tag processing */
struct TagInfo {
	FILE * stream;
	int dataLength;
	int idSize;
};
typedef struct TagInfo TagInfo;
TagInfo makeTagInfo(FILE * f, int dataLength, int idSize);

#endif