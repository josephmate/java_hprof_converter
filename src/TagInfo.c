#include "TagInfo.h"

TagInfo makeTagInfo(FILE * f, int dataLength, int idSize)
{
	TagInfo res;
	res.stream = f;
	res.dataLength = dataLength;
	res.idSize = idSize;
	return res;
}