#include "TagInfo.h"

TagInfo makeTagInfo(FILE * f, long long dataLength, int idSize)
{
	TagInfo res;
	res.stream = f;
	res.dataLength = dataLength;
	res.idSize = idSize;
	return res;
}