// ConsoleApplication1.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <fcntl.h>  
#include <io.h> 
#endif
#include "StreamUtil.h"

#define SIZE_OF_HEADER 31
#define TRUE 1

/* Contains all the info necessary for tag processing */
struct TagInfo {
	FILE * stream;
	int dataLength;
	int idSize;
};
typedef struct TagInfo TagInfo;


int printHeader(unsigned char * header, int* sizeOfId);
int printTag(unsigned char tagType, FILE * f, int idSize);
TagInfo makeTagInfo(FILE * f, int dataLength, int idSize);
int selectAndProcessTag(unsigned char tagType, TagInfo tagInfo);
int processTagString(TagInfo tagInfo);
int processTagLoadClass(TagInfo tagInfo);
int processTagUnloadClass(FILE * f, int dataLength);
int processTagStackFrame(FILE * f, int dataLength);
int processTagStackTrace(FILE * f, int dataLength);
int processTagAllocSites(FILE * f, int dataLength);
int processTagHeapSummary(FILE * f, int dataLength);
int processTagStartThread(FILE * f, int dataLength);
int processTagEndThread(FILE * f, int dataLength);
int processTagHeapDump(FILE * f, int dataLength);
int processTagHeapSegment(FILE * f, int dataLength);
int processTagHeapDumpEnd(FILE * f, int dataLength);
int processTagCpuSamples(FILE * f, int dataLength);
int processTagControlSettings(FILE * f, int dataLength);

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
	fprintf(stdout, "tag type: %d\n", (int)tagType);

	chunksRead = fread(buff, TAG_HEADER_SIZE, 1, f);
	if (chunksRead != 1) {
		fprintf(stderr,
			"Was reading tag header. expected %d bytes, but was %d\n",
			TAG_HEADER_SIZE, (int)chunksRead
		);
	}
	fprintf(stdout, "char array: %d %d %d %d         %d %d %d %d\n",
		buff[0], buff[1], buff[2], buff[3],
		buff[4], buff[5], buff[6], buff[7]
	);

	unsigned int microsSince = fromBigEndianStreamToInt(buffPointer);
	fprintf(stdout, "micros since start: %d\n", microsSince);
	unsigned int dataLength = fromBigEndianStreamToInt(buffPointer +4);
	fprintf(stdout, "data length: %d\n", dataLength);
	
	return selectAndProcessTag(tagType, makeTagInfo(f, dataLength, idSize));
} 

TagInfo makeTagInfo(FILE * f, int dataLength, int idSize)
{
	TagInfo res;
	res.stream = f;
	res.dataLength = dataLength;
	res.idSize = idSize;
	return res;
}


#define TAG_STRING            0x1
#define TAG_LOAD_CLASS        0x2
#define TAG_UNLOAD_CLASS      0x3
#define TAG_STACK_FRAME       0x4
#define TAG_STACK_TRACE       0x5
#define TAG_ALLOC_SITES       0x6
#define TAG_HEAP_SUMMARY      0x7
#define TAG_START_THREAD      0xA
#define TAG_END_THREAD        0xB
#define TAG_HEAP_DUMP         0xC
#define TAG_HEAP_DUMP_SEGMENT 0x1C
#define TAG_HEAP_DUMP_END     0x2C
#define TAG_CPU_SAMPLES       0xD
#define TAG_CONTROL_SETTINGS  0xE
int selectAndProcessTag(unsigned char tagType, TagInfo tagInfo) {
	switch (tagType)
	{
	case TAG_STRING:
		return processTagString(tagInfo);
	case TAG_LOAD_CLASS:
		return processTagLoadClass(tagInfo);
	case TAG_UNLOAD_CLASS:
		return processTagUnloadClass(tagInfo.stream, tagInfo.dataLength);
	case TAG_STACK_FRAME:
		return processTagStackFrame(tagInfo.stream, tagInfo.dataLength);
	case TAG_STACK_TRACE:
		return processTagStackTrace(tagInfo.stream, tagInfo.dataLength);
	case TAG_ALLOC_SITES:
		return processTagAllocSites(tagInfo.stream, tagInfo.dataLength);
	case TAG_HEAP_SUMMARY:
		return processTagHeapSummary(tagInfo.stream, tagInfo.dataLength);
	case TAG_START_THREAD:
		return processTagStartThread(tagInfo.stream, tagInfo.dataLength);
	case TAG_END_THREAD:
		return processTagEndThread(tagInfo.stream, tagInfo.dataLength);
	case TAG_HEAP_DUMP:
		return processTagHeapDump(tagInfo.stream, tagInfo.dataLength);
	case TAG_HEAP_DUMP_SEGMENT:
		return processTagHeapSegment(tagInfo.stream, tagInfo.dataLength);
	case TAG_HEAP_DUMP_END:
		return processTagHeapDumpEnd(tagInfo.stream, tagInfo.dataLength);
	case TAG_CPU_SAMPLES:
		return processTagCpuSamples(tagInfo.stream, tagInfo.dataLength);
	case TAG_CONTROL_SETTINGS:
		return processTagControlSettings(tagInfo.stream, tagInfo.dataLength);
	default:
		fprintf(stdout, "tag not recognized or implemented: %d", tagType);
		return iterateThroughStream(tagInfo.stream, tagInfo.dataLength);
	}
}

/**
 * ID ID for this string
 * [u1]* UTF8 characters for string (NOT NULL terminated)
 */
int processTagString(TagInfo tagInfo) {
	fprintf(stdout, "TAG_STRING\n");
	unsigned long long id;
	if (getId(tagInfo.stream, tagInfo.idSize, &id) != 0) {
		return -1;
	}
	fprintf(stdout, "id: %lld\n", id);
	// idSize less because we already read idSize bytes from the stream
	// +1 because we need a null character at the end
	int strLen = tagInfo.dataLength - tagInfo.idSize + 1;
	char * str = malloc(sizeof(char)*strLen);
	str[strLen - 1] = '\0';
	//one less because last char is the null character
	if (fread(str, strLen - 1, 1, tagInfo.stream) != 1) {
		fprintf(stderr, "expected to read %d bytes for TAG_STRING's string, but failed\n", strLen - 1);
	}
	fprintf(stdout, "str: %s\n", str);
	free(str);
	return 0;
}

/**
 * u4 class serial number (always > 0)
 * ID class object ID
 * u4 stack trace serial number
 * ID class name string ID
 */
int processTagLoadClass(TagInfo tagInfo) {
	fprintf(stdout, "TAG_LOAD_CLASS\n"); 
	int totalRequiredBytes = 2 * 4 + 2 * tagInfo.idSize;
	if (tagInfo.dataLength < totalRequiredBytes) {
		fprintf(stderr, "TAG_LOAD_CLASS required %d bytes but we only got %d.\n", totalRequiredBytes, tagInfo.dataLength);
	}

	int classSerialNumber;
	if (readBigEndianStreamToInt(tagInfo.stream, &classSerialNumber) != 0) {
		fprintf(stderr, "unable to read u4 class serial number for TAG_LOAD_CLASS\n");
		return -1;
	}

	unsigned long long classObjId;
	if (getId(tagInfo.stream, tagInfo.idSize, &classObjId) != 0) {
		return -1;
	}

	int stackTraceSerialNumber;
	if (readBigEndianStreamToInt(tagInfo.stream, &stackTraceSerialNumber) != 0) {
		fprintf(stderr, "unable to read u4 class serial number for TAG_LOAD_CLASS\n");
		return -1;
	}

	unsigned long long classNameStringId;
	if (getId(tagInfo.stream, tagInfo.idSize, &classNameStringId) != 0) {
		return -1;
	}

	fprintf(stdout, "classSerialNumber: %d\n", classSerialNumber);
	fprintf(stdout, "classObjId: %lld\n", classObjId);
	fprintf(stdout, "stackTraceSerialNumber: %d\n", stackTraceSerialNumber);
	fprintf(stdout, "classNameStringId: %lld\n", classNameStringId);

	return 0;
}

/**
 * u4 class serial number
 */
int processTagUnloadClass(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_UNLOAD_CLASS\n");
	// TODO
	return iterateThroughStream(f, dataLength);
}

/**
 * ID stack frame ID
 * ID method name string ID
 * ID method signature string ID
 * ID source file name string ID
 * u4 class serial number
 * u4 >  0 line number
 *    =  0 no line information available
 *    = -1 unknown location
 *    = -2 compiled method (Not implemented)
 *    = -3 native method (Not implemented)
 */
int processTagStackFrame(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_STACK_FRAME\n");
	// TODO
	return iterateThroughStream(f, dataLength);
}

/**
 * u4 stack trace serial number
 * u4 thread serial number
 * u4 number of frames
 * [ID]* series of stack frame ID's
 */
int processTagStackTrace(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_STACK_TRACE\n");
	// TODO
	return iterateThroughStream(f, dataLength);
}

/**
 * u2 Bit mask flags:
 * 0x1 incremental vs. complete
 * 0x2 sorted by allocation vs. line
 * 0x4 whether to force GC (Not Implemented)
 *
 * u4 cutoff ratio (floating point)
 * u4 total live bytes
 * u4 total live instances
 * u8 total bytes allocated
 * u8 total instances allocated
 * u4 number of sites that follow:
 *   u1 array indicator: 0 means not an array, non-zero means an array of this type (See Basic Type)
 *   u4 class serial number
 *   u4 stack trace serial number
 *   u4 number of live bytes
 *   u4 number of live instances
 *   u4 number of bytes allocated
 *   u4 number of instances allocated
 */
int processTagAllocSites(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_ALLOC_SITES\n");
	// TODO
	return iterateThroughStream(f, dataLength);
}

/**
 * u4 total live bytes
 * u4 total live instances
 * u8 total bytes allocated
 * u8 total instances allocated
 */
int processTagHeapSummary(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_HEAP_SUMMARY\n");
	// TODO
	return iterateThroughStream(f, dataLength);
}

/**
u4 thread serial number
ID thread object ID
u4 stack trace serial number
ID thread name string ID
ID thread group name ID
ID thread parent group name ID
 */
int processTagStartThread(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_START_THREAD\n");
	// TODO
	return iterateThroughStream(f, dataLength);
}

/**
u4 thread serial number
 */
int processTagEndThread(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_END_THREAD\n");
	// TODO
	return iterateThroughStream(f, dataLength);
}

/**
 * too complex probably deserves it's own class
 */
int processTagHeapDump(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_HEAP_DUMP\n");
	// TODO
	return iterateThroughStream(f, dataLength);
}

/**
 * see processTagHeapSegment
 */
int processTagHeapSegment(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_HEAP_DUMP_SEGMENT\n");
	// TODO
	return iterateThroughStream(f, dataLength);
}

/**
 * Terminates a series of HEAP DUMP SEGMENTS.  Concatenation of HEAP DUMP SEGMENTS equals a HEAP DUMP.
 */
int processTagHeapDumpEnd(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_HEAP_DUMP_END\n");
	// TODO
	return iterateThroughStream(f, dataLength);
}

/**
u4 total number of samples
u4 number of traces that follow:
u4 number of samples
u4 stack trace serial number
 */
int processTagCpuSamples(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_CPU_SAMPLES\n");
	// TODO
	return iterateThroughStream(f, dataLength);
}

/**
u4 Bit mask flags:
    0x1 alloc traces on/off
    0x2 cpu sampling on/off

u2 stack trace depth
 */
int processTagControlSettings(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_CONTROL_SETTINGS\n");
	// TODO
	return iterateThroughStream(f, dataLength);
}