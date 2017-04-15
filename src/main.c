// ConsoleApplication1.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#ifdef _WIN32
#include <fcntl.h>  
#include <io.h> 
#endif
#include "StreamUtil.h"
#include "TagInfo.h"
#include "ProcessTags.h"

#define SIZE_OF_HEADER 31
#define TRUE 1



int printHeader(unsigned char * header, int* sizeOfId);
int printTag(unsigned char tagType, FILE * f, int idSize);

int main()
{	
	size_t numOfBytesRead;
	FILE * f = stdin;
	char buff[SIZE_OF_HEADER];
	char * buffPointer = buff;

#ifdef _WIN32
	int result = _setmode(_fileno(stdin), _O_BINARY);
	if (result == -1)
		perror("Cannot set mode");
	else
		printf("'stdin' successfully changed to binary mode\n");
#endif


	// read the header
	numOfBytesRead = fread(buffPointer, SIZE_OF_HEADER, 1, f);
	if (numOfBytesRead != 1) {
		fprintf(stderr, "Was expecting the hprof binary file to contain at least %d bytes, but we were only able to read some of it\n",
			SIZE_OF_HEADER);
		return -1;
	}
	int idSize;
	printHeader(buffPointer, &idSize);

	// read the tag until there are no more
	while(TRUE) {
		numOfBytesRead = fread(buffPointer, 1, 1, f);
		if (numOfBytesRead < 1) {
			break;
		}

		int tagErrorCode = printTag(buff[0], f, idSize);
		if (tagErrorCode != 0) {
			return tagErrorCode;
		}
	}
	// no more bytes to read
	fprintf(stdout, "no more bytes to read\n");
	
	
    return 0;
}

/**
 * header is a 31 byte array containing the header data
 * [u1]  An initial NULL terminated series of bytes representing the format name and version,
 *       in this implementation and historically, the string "JAVA PROFILE 1.0.1" (18 u1 bytes)
 *       followed by a NULL byte. If the TAG "HEAP DUMP SEGMENT" is used this string will be
 *       "JAVA PROFILE 1.0.2".
 * u4 size of identifiers. Identifiers are used to represent UTF8 strings, objects, stack traces,
 *    etc. They can have the same size as host pointers or sizeof(void*), but are not required to
 *    be.
 * u4 high word of number of milliseconds since 0:00 GMT, 1/1/70
 * u4 low word of number of milliseconds since 0:00 GMT, 1/1/70
 */
int printHeader(unsigned char * header, int* idSize) {
	int nullposn = posnOfNull(header, 31-12);

	if (nullposn < 0) {
		fprintf(stderr, "was expecting a null terminating character in the header, but was not found");
		return -1;
	}

	header = header + nullposn + 1;
	*idSize = fromBigEndianStreamToInt(header);
	fprintf(stdout, "size of id: %d\n", *idSize);
	header += 4;
	int highWordMicrosSinceEpoch = fromBigEndianStreamToInt(header);
	header += 4;
	int lowWordMicrosSinceEpoch = fromBigEndianStreamToInt(header);
	fprintf(stdout, "high word of micros since epoch: %d\n", highWordMicrosSinceEpoch);
	fprintf(stdout, "low  word of micros since epoch: %d\n", lowWordMicrosSinceEpoch);
	return 0;
}


#define TAG_HEADER_SIZE 8
int printTag(unsigned char tagType, FILE * f, int idSize) {
	unsigned char buff[TAG_HEADER_SIZE];
	unsigned char * buffPointer = buff;
	size_t chunksRead;
	//fprintf(stdout, "tag type: %d\n", (int)tagType);

	chunksRead = fread(buff, TAG_HEADER_SIZE, 1, f);
	if (chunksRead != 1) {
		fprintf(stderr,
			"Was reading tag header. expected %d bytes, but was %d\n",
			TAG_HEADER_SIZE, (int)chunksRead
		);
	}
	/* for debugging
	fprintf(stdout, "char array: %d %d %d %d         %d %d %d %d\n",
		buff[0], buff[1], buff[2], buff[3],
		buff[4], buff[5], buff[6], buff[7]
	);
	*/

	unsigned int microsSince = fromBigEndianStreamToInt(buffPointer);
	fprintf(stdout, "microsSinceStart:%d, ", microsSince);
	unsigned int dataLength = fromBigEndianStreamToInt(buffPointer +4);
	//fprintf(stdout, "data length: %d\n", dataLength);
	
	return selectAndProcessTag(tagType, makeTagInfo(f, dataLength, idSize));
} 

