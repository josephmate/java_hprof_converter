// ConsoleApplication1.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#ifdef _WIN32
#include <fcntl.h>  
#endif

#define SIZE_OF_HEADER 31
#define TRUE 1

void print_header(char * header);
int print_tag(unsigned char tagType, FILE * f);
int fromLittleEndianStreamToInt(unsigned char * data);
int fromBigEndianStreamToInt(unsigned char * data);
int iterateThroughStream(FILE* f, int dataLength);
int selectAndProcessTag(unsigned char tagType, FILE * f, int dataLength);
int processTagString(FILE * f, int dataLength);
int processTagLoadClass(FILE * f, int dataLength);
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
	char * buffPointer = &buff;

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
		fprintf(stderr, "Was expecting the hprof binary file to contain at least %d bytes, but we were only able to read 0\n",
			SIZE_OF_HEADER);
		return -1;
	}
	print_header(buff);

	// read the tag until there are no more
	while(TRUE) {
		numOfBytesRead = fread(buffPointer, 1, 1, f);
		if (numOfBytesRead < 1) {
			break;
		}

		int tagErrorCode = print_tag(buff[0], f);
		if (tagErrorCode != 0) {
			return tagErrorCode;
		}
	}
	// no more bytes to read
	fprintf(stdout, "no more bytes to read\n");  fflush(stdout);
	
	
    return 0;
}

/**
 * header is a 31 byte array containing the header data
 */
void print_header(char * header) {
	// TODO
}

#define TAG_HEADER_SIZE 8
int print_tag(unsigned char tagType, FILE * f) {
	unsigned char buff[TAG_HEADER_SIZE];
	unsigned char * buffPointer = &buff;
	size_t chunksRead;
	fprintf(stdout, "tag type: %d\n", (int)tagType);  fflush(stdout);

	chunksRead = fread(buff, TAG_HEADER_SIZE, 1, f);
	if (chunksRead != 1) {
		fprintf(stderr,
			"Was reading tag header. expected %d bytes, but was %d\n",
			TAG_HEADER_SIZE, chunksRead
		);
	}
	fprintf(stdout, "char array: %d %d %d %d         %d %d %d %d\n",
		buff[0], buff[1], buff[2], buff[3],
		buff[4], buff[5], buff[6], buff[7]
	);
	fflush(stdout);

	int microsSince = fromBigEndianStreamToInt(buffPointer);
	fprintf(stdout, "micros since start: %d\n", microsSince); fflush(stdout);
	int dataLength = fromBigEndianStreamToInt(buffPointer +4);
	fprintf(stdout, "data length: %d\n", dataLength); fflush(stdout);
	
	return selectAndProcessTag(tagType, f, dataLength);
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
int selectAndProcessTag(unsigned char tagType, FILE * f, int dataLength) {
	switch (tagType)
	{
	case TAG_STRING:
		return processTagString(f, dataLength);
	case TAG_LOAD_CLASS:
		return processTagLoadClass(f, dataLength);
	case TAG_UNLOAD_CLASS:
		return processTagUnloadClass(f, dataLength);
	case TAG_STACK_FRAME:
		return processTagStackFrame(f, dataLength);
	case TAG_STACK_TRACE:
		return processTagStackTrace(f, dataLength);
	case TAG_ALLOC_SITES:
		return processTagAllocSites(f, dataLength);
	case TAG_HEAP_SUMMARY:
		return processTagHeapSummary(f, dataLength);
	case TAG_START_THREAD:
		return processTagStartThread(f, dataLength);
	case TAG_END_THREAD:
		return processTagEndThread(f, dataLength);
	case TAG_HEAP_DUMP:
		return processTagHeapDump(f, dataLength);
	case TAG_HEAP_DUMP_SEGMENT:
		return processTagHeapSegment(f, dataLength);
	case TAG_HEAP_DUMP_END:
		return processTagHeapDumpEnd(f, dataLength);
	case TAG_CPU_SAMPLES:
		return processTagCpuSamples(f, dataLength);
	case TAG_CONTROL_SETTINGS:
		return processTagControlSettings(f, dataLength);
	default:
		fprintf(stdout, "tag not recognized or implemented: %d", tagType);
		return iterateThroughStream(f, dataLength);
	}
}

/**
 * ID ID for this string
 * [u1]* UTF8 characters for string (NOT NULL terminated)
 */
int processTagString(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_STRING\n");  fflush(stdout);
	// TODO
	return iterateThroughStream(f, dataLength);
}

/**
 * u4 class serial number (always > 0)
 * ID class object ID
 * u4 stack trace serial number
 * ID class name string ID
 */
int processTagLoadClass(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_LOAD_CLASS\n");  fflush(stdout);
	// TODO
	return iterateThroughStream(f, dataLength);
}

/**
 * u4 class serial number
 */
int processTagUnloadClass(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_UNLOAD_CLASS\n");  fflush(stdout);
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
	fprintf(stdout, "TAG_STACK_FRAME\n");  fflush(stdout);
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
	fprintf(stdout, "TAG_STACK_TRACE\n");  fflush(stdout);
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
	fprintf(stdout, "TAG_ALLOC_SITES\n");  fflush(stdout);
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
	fprintf(stdout, "TAG_HEAP_SUMMARY\n");  fflush(stdout);
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
	fprintf(stdout, "TAG_START_THREAD\n");  fflush(stdout);
	// TODO
	return iterateThroughStream(f, dataLength);
}

/**
u4 thread serial number
 */
int processTagEndThread(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_END_THREAD\n");  fflush(stdout);
	// TODO
	return iterateThroughStream(f, dataLength);
}

/**
 * too complex probably deserves it's own class
 */
int processTagHeapDump(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_HEAP_DUMP\n");  fflush(stdout);
	// TODO
	return iterateThroughStream(f, dataLength);
}

/**
 * see processTagHeapSegment
 */
int processTagHeapSegment(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_HEAP_DUMP_SEGMENT\n");  fflush(stdout);
	// TODO
	return iterateThroughStream(f, dataLength);
}

/**
 * Terminates a series of HEAP DUMP SEGMENTS.  Concatenation of HEAP DUMP SEGMENTS equals a HEAP DUMP.
 */
int processTagHeapDumpEnd(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_HEAP_DUMP_END\n");  fflush(stdout);
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
	fprintf(stdout, "TAG_CPU_SAMPLES\n");  fflush(stdout);
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
	fprintf(stdout, "TAG_CONTROL_SETTINGS\n");  fflush(stdout);
	// TODO
	return iterateThroughStream(f, dataLength);
}

/**
 * Iterates through dataLength bytes of the stream: f.
 */
int iterateThroughStream(FILE* f, int dataLength) {
	unsigned char buff[1];
	unsigned char * buffPointer = &buff;
	size_t numOfBytesRead;
	for (int i = 0; i < dataLength; i++) {
		numOfBytesRead = fread(buffPointer, 1, 1, f);
		if (numOfBytesRead != 1) {
			fprintf(stderr,
				"Was reading data of tag entry. expected %d bytes, but failed on %d\n",
				dataLength, i
			);
			perror("The following error occurred:");
			return -1;
		}
	}
	return 0;
}

// http://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c
int fromLittleEndianStreamToInt(unsigned char * data) {
	return  (data[0] << 0) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
}
int fromBigEndianStreamToInt(unsigned char * data) {
	return  (data[3] << 0) | (data[2] << 8) | (data[1] << 16) | (data[0] << 24);
}