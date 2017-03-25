
#include "ProcessTags.h"
#include "ProcessTagHeap.h"
#include "StreamUtil.h"
#include "TagInfo.h"
#include <stdlib.h>

int processTagStackTraceFrame(TagInfo tagInfo);
int processTagCpuSampleTrace(TagInfo tagInfo);
int processATagAllocSite(TagInfo tagInfo);

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
		return processTagUnloadClass(tagInfo);
	case TAG_STACK_FRAME:
		return processTagStackFrame(tagInfo);
	case TAG_STACK_TRACE:
		return processTagStackTrace(tagInfo);
	case TAG_ALLOC_SITES:
		return processTagAllocSites(tagInfo);
	case TAG_HEAP_SUMMARY:
		return processTagHeapSummary(tagInfo);
	case TAG_START_THREAD:
		return processTagStartThread(tagInfo);
	case TAG_END_THREAD:
		return processTagEndThread(tagInfo);
	case TAG_HEAP_DUMP:
		return processTagHeapDump(tagInfo.stream, tagInfo.dataLength);
	case TAG_HEAP_DUMP_SEGMENT:
		return processTagHeapSegment(tagInfo.stream, tagInfo.dataLength);
	case TAG_HEAP_DUMP_END:
		return processTagHeapDumpEnd(tagInfo);
	case TAG_CPU_SAMPLES:
		return processTagCpuSamples(tagInfo);
	case TAG_CONTROL_SETTINGS:
		return processTagControlSettings(tagInfo);
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
	char * str = (char*)malloc(sizeof(char)*strLen);
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
		return -1;
	}

	unsigned int classSerialNumber;
	if (readBigEndianStreamToInt(tagInfo.stream, &classSerialNumber) != 0) {
		fprintf(stderr, "unable to read u4 class serial number for TAG_LOAD_CLASS\n");
		return -1;
	}

	unsigned long long classObjId;
	if (getId(tagInfo.stream, tagInfo.idSize, &classObjId) != 0) {
		fprintf(stderr, "unable to read %d bytes classObjId for TAG_LOAD_CLASS\n", tagInfo.idSize);
		return -1;
	}

	unsigned int stackTraceSerialNumber;
	if (readBigEndianStreamToInt(tagInfo.stream, &stackTraceSerialNumber) != 0) {
		fprintf(stderr, "unable to read u4 stackTraceSerialNumber for TAG_LOAD_CLASS\n");
		return -1;
	}

	unsigned long long classNameStringId;
	if (getId(tagInfo.stream, tagInfo.idSize, &classNameStringId) != 0) {
		fprintf(stderr, "unable to read %d bytes classNameStringId for TAG_LOAD_CLASS\n", tagInfo.idSize);
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
int processTagUnloadClass(TagInfo tagInfo) {
	fprintf(stdout, "TAG_UNLOAD_CLASS\n");


	int totalRequiredBytes = 1 * 4;
	if (tagInfo.dataLength != totalRequiredBytes) {
		fprintf(stderr, "TAG_UNLOAD_CLASS required %d bytes but we only got %d.\n", totalRequiredBytes, tagInfo.dataLength);
	}

	unsigned int classSerialNumber;
	if (readBigEndianStreamToInt(tagInfo.stream, &classSerialNumber) != 0) {
		fprintf(stderr, "unable to read u4 classSerialNumber for TAG_UNLOAD_CLASS\n");
		return -1;
	}

	fprintf(stdout, "classSerialNumber: %d\n", classSerialNumber);

	return 0;
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
int processTagStackFrame(TagInfo tagInfo) {
	fprintf(stdout, "TAG_STACK_FRAME\n");
	int totalRequiredBytes = 2 * 4 + 4 * tagInfo.idSize;
	if (tagInfo.dataLength != totalRequiredBytes) {
		fprintf(stderr, "TAG_STACK_FRAME required %d bytes but we only got %d.\n", totalRequiredBytes, tagInfo.dataLength);
	}



	unsigned long long stackFrameId;
	if (getId(tagInfo.stream, tagInfo.idSize, &stackFrameId) != 0) {
		fprintf(stderr, "unable to read %d bytes stackFrameId for TAG_STACK_FRAME\n", tagInfo.idSize);
		return -1;
	}

	unsigned long long methodNameStringId;
	if (getId(tagInfo.stream, tagInfo.idSize, &methodNameStringId) != 0) {
		fprintf(stderr, "unable to read %d bytes methodNameStringId for TAG_STACK_FRAME\n", tagInfo.idSize);
		return -1;
	}

	unsigned long long methodSignatureStringId;
	if (getId(tagInfo.stream, tagInfo.idSize, &methodSignatureStringId) != 0) {
		fprintf(stderr, "unable to read %d bytes methodSignatureStringId for TAG_STACK_FRAME\n", tagInfo.idSize);
		return -1;
	}

	unsigned long long sourceFileNameStringId;
	if (getId(tagInfo.stream, tagInfo.idSize, &sourceFileNameStringId) != 0) {
		fprintf(stderr, "unable to read %d bytes sourceFileNameStringId for TAG_STACK_FRAME\n", tagInfo.idSize);
		return -1;
	}



	unsigned int classSerialNumber;
	if (readBigEndianStreamToInt(tagInfo.stream, &classSerialNumber) != 0) {
		fprintf(stderr, "unable to read u4 classSerialNumber for TAG_STACK_FRAME\n");
		return -1;
	}

	unsigned int lineInfo;
	if (readBigEndianStreamToInt(tagInfo.stream, &lineInfo) != 0) {
		fprintf(stderr, "unable to read u4 lineInfo for TAG_STACK_FRAME\n");
		return -1;
	}


	fprintf(stdout, "stackFrameId:            %lld\n", stackFrameId);
	fprintf(stdout, "methodNameStringId:      %lld\n", methodNameStringId);
	fprintf(stdout, "methodSignatureStringId: %lld\n", methodSignatureStringId);
	fprintf(stdout, "sourceFileNameStringId:  %lld\n", sourceFileNameStringId);
	fprintf(stdout, "classSerialNumber:       %d\n", classSerialNumber);
	fprintf(stdout, "lineInfo:                %d\n", lineInfo);

	return 0;
}

/**
* u4 stack trace serial number
* u4 thread serial number
* u4 number of frames
* [ID]* series of stack frame ID's
*/
int processTagStackTrace(TagInfo tagInfo) {
	fprintf(stdout, "TAG_STACK_TRACE\n");
	int totalRequiredBytes = 3 * 4;
	if (tagInfo.dataLength < totalRequiredBytes) {
		fprintf(stderr, "TAG_STACK_TRACE required %d bytes but we only got %d.\n", totalRequiredBytes, tagInfo.dataLength);
		return -1;
	}

	unsigned int stackTraceSerialNumber;
	if (readBigEndianStreamToInt(tagInfo.stream, &stackTraceSerialNumber) != 0) {
		fprintf(stderr, "unable to read u4 stackTraceSerialNumber for TAG_STACK_FRAME\n");
		return -1;
	}
	unsigned int threadSerialNumber;
	if (readBigEndianStreamToInt(tagInfo.stream, &threadSerialNumber) != 0) {
		fprintf(stderr, "unable to read u4 threadSerialNumber for TAG_STACK_FRAME\n");
		return -1;
	}
	unsigned int numberOfFrames;
	if (readBigEndianStreamToInt(tagInfo.stream, &numberOfFrames) != 0) {
		fprintf(stderr, "unable to read u4 numberOfFrames for TAG_STACK_FRAME\n");
		return -1;
	}

	fprintf(stdout, "stackTraceSerialNumber:   %d\n", stackTraceSerialNumber);
	fprintf(stdout, "threadSerialNumber:       %d\n", threadSerialNumber);

	int dataLeft = tagInfo.dataLength - totalRequiredBytes;
	if (dataLeft != numberOfFrames * tagInfo.idSize) {
		fprintf(stderr, "TAG_STACK_TRACE required %d bytes for frames but we only got %d.\n", totalRequiredBytes, dataLeft);
	}

	
	for (unsigned int i = 0; i < numberOfFrames; i++) {
		int errCode = processTagStackTraceFrame(tagInfo);
		if (errCode  != 0) {
			fprintf(stderr, "TAG_STACK_TRACE failed on %d th stack trace frame\n", i);
			return errCode;
		}
	}

	return 0;
}

int processTagStackTraceFrame(TagInfo tagInfo) {
	unsigned long long stackFrameId;
	if (getId(tagInfo.stream, tagInfo.idSize, &stackFrameId) != 0) {
		fprintf(stderr, "unable to read %d bytes stackFrameId for TAG_STACK_FRAME\n", tagInfo.idSize);
		return -1;
	}
	fprintf(stdout, "stackFrameId:  %lld\n", stackFrameId);
	return 0;
}

/**
* u2 Bit mask flags:
*   0x1 incremental vs. complete
*   0x2 sorted by allocation vs. line
*   0x4 whether to force GC (Not Implemented)
*
* u4 cutoff ratio (floating point)
* u4 total live bytes
* u4 total live instances
* u8 total bytes allocated
* u8 total instances allocated
* u4 number of sites that follow:
*   for each:
*     u1 array indicator: 0 means not an array, non-zero means an array of this type (See Basic Type)
*     u4 class serial number
*     u4 stack trace serial number
*     u4 number of live bytes
*     u4 number of live instances
*     u4 number of bytes allocated
*     u4 number of instances allocated
*/
int processTagAllocSites(TagInfo tagInfo) {
	fprintf(stdout, "TAG_ALLOC_SITES\n");
	unsigned int totalRequiredBytes = 4 * 4 + 8 * 2;
	if (tagInfo.dataLength < totalRequiredBytes) {
		fprintf(stderr, "TAG_ALLOC_SITES required %d bytes but we only got %d.\n", totalRequiredBytes, tagInfo.dataLength);
		return -1;
	}

	unsigned int  gcFlags;
	if (readTwoByteBigEndianStreamToInt(tagInfo.stream, &gcFlags) != 0) {
		fprintf(stderr, "unable to read u2 gcFlags for TAG_ALLOC_SITES\n");
		return -1;
	}


	unsigned int  rawCutoffRatio;
	if (readBigEndianStreamToInt(tagInfo.stream, &rawCutoffRatio) != 0) {
		fprintf(stderr, "unable to read u4 rawCutoffRatio for TAG_ALLOC_SITES\n");
		return -1;
	}
	float cutoffRatio = (float)rawCutoffRatio;
	unsigned int totalLiveBytes;
	if (readBigEndianStreamToInt(tagInfo.stream, &totalLiveBytes) != 0) {
		fprintf(stderr, "unable to read u4 totalLiveBytes for TAG_ALLOC_SITES\n");
		return -1;
	}
	unsigned int totalLiveInstances;
	if (readBigEndianStreamToInt(tagInfo.stream, &totalLiveInstances) != 0) {
		fprintf(stderr, "unable to read u4 totalLiveInstances for TAG_ALLOC_SITES\n");
		return -1;
	}

	unsigned long long totalBytesAllocated;
	if (readBigWordSmallWordBigEndianStreamToLong(tagInfo.stream, &totalBytesAllocated) != 0) {
		fprintf(stderr, "unable to read u8 totalBytesAllocated for TAG_ALLOC_SITES\n");
		return -1;
	}	unsigned long long totalInstancesAllocated;
	if (readBigWordSmallWordBigEndianStreamToLong(tagInfo.stream, &totalInstancesAllocated) != 0) {
		fprintf(stderr, "unable to read u8 totalInstancesAllocated for TAG_ALLOC_SITES\n");
		return -1;
	}

	fprintf(stdout, "gcFlags:                 %d\n", gcFlags);
	fprintf(stdout, "rawCutoffRatio:          %d\n", rawCutoffRatio);
	fprintf(stdout, "cutoffRatio:             %f\n", cutoffRatio);
	fprintf(stdout, "totalLiveBytes:          %d\n", totalLiveBytes);
	fprintf(stdout, "totalLiveInstances:      %d\n", totalLiveInstances);
	fprintf(stdout, "totalBytesAllocated:     %lld\n", totalBytesAllocated);
	fprintf(stdout, "totalInstancesAllocated: %lld\n", totalInstancesAllocated);

	
	unsigned int numberOfSites;
	if (readBigEndianStreamToInt(tagInfo.stream, &numberOfSites) != 0) {
		fprintf(stderr, "unable to read u4 numberOfFrames for TAG_ALLOC_SITES\n");
		return -1;
	}
	unsigned int bytesLeft = tagInfo.dataLength - totalRequiredBytes;
	totalRequiredBytes = numberOfSites*(1 * 1 + 4 * 6);
	if (bytesLeft < totalRequiredBytes) {
		fprintf(stderr, "TAG_ALLOC_SITES required %d bytes for frames but we only got %d.\n", totalRequiredBytes, bytesLeft);
	}

	for (unsigned int i = 0; i < numberOfSites; i++) {
		int errorCode = processATagAllocSite(tagInfo);
		if (errorCode != 0) {
			fprintf(stderr, "TAG_ALLOC_SITES failed on %d th site.\n", i);
			return -1;
		}
	}

	return 0;
}

/*
*     u1 array indicator: 0 means not an array, non-zero means an array of this type (See Basic Type)
*     u4 class serial number
*     u4 stack trace serial number
*     u4 number of live bytes
*     u4 number of live instances
*     u4 number of bytes allocated
*     u4 number of instances allocated
*/
int processATagAllocSite(TagInfo tagInfo) {
	unsigned int arrayIndicator;
	if (readByteToInt(tagInfo.stream, &arrayIndicator) != 0) {
		fprintf(stderr, "unable to read u1 arrayIndicator for TAG_ALLOC_SITES\n");
		return -1;
	}


	unsigned int classSerialNumber;
	if (readBigEndianStreamToInt(tagInfo.stream, &classSerialNumber) != 0) {
		fprintf(stderr, "unable to read u4 classSerialNumber for TAG_ALLOC_SITES\n");
		return -1;
	}
	unsigned int stackTraceSerialNumber;
	if (readBigEndianStreamToInt(tagInfo.stream, &stackTraceSerialNumber) != 0) {
		fprintf(stderr, "unable to read u4 stackTraceSerialNumber for TAG_ALLOC_SITES\n");
		return -1;
	}

	unsigned int numberLiveBytes;
	if (readBigEndianStreamToInt(tagInfo.stream, &numberLiveBytes) != 0) {
		fprintf(stderr, "unable to read u4 numberLiveBytes for TAG_ALLOC_SITES\n");
		return -1;
	}
	unsigned int numberLiveInstances;
	if (readBigEndianStreamToInt(tagInfo.stream, &numberLiveInstances) != 0) {
		fprintf(stderr, "unable to read u4 numberLiveInstances for TAG_ALLOC_SITES\n");
		return -1;
	}

	unsigned int numberBytesAllocated;
	if (readBigEndianStreamToInt(tagInfo.stream, &numberBytesAllocated) != 0) {
		fprintf(stderr, "unable to read u4 numberBytesAllocated for TAG_ALLOC_SITES\n");
		return -1;
	}
	unsigned int numberInstancesAllocated;
	if (readBigEndianStreamToInt(tagInfo.stream, &numberInstancesAllocated) != 0) {
		fprintf(stderr, "unable to read u4 numberInstancesAllocated for TAG_ALLOC_SITES\n");
		return -1;
	}

	return 0;
}

/**
* u4 total live bytes
* u4 total live instances
* u8 total bytes allocated
* u8 total instances allocated
*/
int processTagHeapSummary(TagInfo tagInfo) {
	fprintf(stdout, "TAG_HEAP_SUMMARY\n");
	int totalRequiredBytes = 2 * 4 + 2 * 8;
	if (tagInfo.dataLength != totalRequiredBytes) {
		fprintf(stderr, "TAG_HEAP_SUMMARY required %d bytes but we only got %d.\n", totalRequiredBytes, tagInfo.dataLength);
	}



	unsigned int totalLiveBytes;
	if (readBigEndianStreamToInt(tagInfo.stream, &totalLiveBytes) != 0) {
		fprintf(stderr, "unable to read u4 totalLiveBytes for TAG_HEAP_SUMMARY\n");
		return -1;
	}
	unsigned int totalLiveInstances;
	if (readBigEndianStreamToInt(tagInfo.stream, &totalLiveInstances) != 0) {
		fprintf(stderr, "unable to read u4 totalLiveInstances for TAG_HEAP_SUMMARY\n");
		return -1;
	}


	unsigned long long totalBytesAllocated;
	if (readBigWordSmallWordBigEndianStreamToLong(tagInfo.stream, &totalBytesAllocated) != 0) {
		fprintf(stderr, "unable to read u8 totalBytesAllocated for TAG_HEAP_SUMMARY\n");
		return -1;
	}
	unsigned long long totalInstancesAllocated;
	if (readBigWordSmallWordBigEndianStreamToLong(tagInfo.stream, &totalInstancesAllocated) != 0) {
		fprintf(stderr, "unable to read u8 totalInstancesAllocated for TAG_HEAP_SUMMARY\n");
		return -1;
	}


	fprintf(stdout, "totalLiveBytes:          %d\n", totalLiveBytes);
	fprintf(stdout, "totalLiveInstances:      %d\n", totalLiveInstances);
	fprintf(stdout, "totalBytesAllocated:     %lld\n", totalBytesAllocated);
	fprintf(stdout, "totalInstancesAllocated: %lld\n", totalInstancesAllocated);

	return 0;
}

/**
u4 thread serial number
ID thread object ID
u4 stack trace serial number
ID thread name string ID
ID thread group name ID
ID thread parent group name ID
*/
int processTagStartThread(TagInfo tagInfo) {
	fprintf(stdout, "TAG_START_THREAD\n");
	int totalRequiredBytes = 2 * 4 + 4 * tagInfo.idSize;
	if (tagInfo.dataLength != totalRequiredBytes) {
		fprintf(stderr, "TAG_START_THREAD required %d bytes but we only got %d.\n", totalRequiredBytes, tagInfo.dataLength);
	}

	unsigned int threadSerialNumber;
	if (readBigEndianStreamToInt(tagInfo.stream, &threadSerialNumber) != 0) {
		fprintf(stderr, "unable to read u4 threadSerialNumber for TAG_START_THREAD\n");
		return -1;
	}
	unsigned long long threadObjectId;
	if (getId(tagInfo.stream, tagInfo.idSize, &threadObjectId) != 0) {
		fprintf(stderr, "unable to read %d bytes threadObjectId for TAG_START_THREAD\n", tagInfo.idSize);
		return -1;
	}

	unsigned int stackTraceSerialNumber;
	if (readBigEndianStreamToInt(tagInfo.stream, &stackTraceSerialNumber) != 0) {
		fprintf(stderr, "unable to read u4 stackTraceSerialNumber for TAG_START_THREAD\n");
		return -1;
	}
	unsigned long long threadNameStringId;
	if (getId(tagInfo.stream, tagInfo.idSize, &threadNameStringId) != 0) {
		fprintf(stderr, "unable to read %d bytes threadNameStringId for TAG_START_THREAD\n", tagInfo.idSize);
		return -1;
	}

	unsigned long long threadGroupNameId;
	if (getId(tagInfo.stream, tagInfo.idSize, &threadGroupNameId) != 0) {
		fprintf(stderr, "unable to read %d bytes threadGroupNameId for TAG_START_THREAD\n", tagInfo.idSize);
		return -1;
	}
	unsigned long long threadParentGroupNameId;
	if (getId(tagInfo.stream, tagInfo.idSize, &threadParentGroupNameId) != 0) {
		fprintf(stderr, "unable to read %d bytes threadParentGroupNameId for TAG_START_THREAD\n", tagInfo.idSize);
		return -1;
	}


	fprintf(stdout, "threadSerialNumber:      %d\n", threadSerialNumber);
	fprintf(stdout, "threadObjectId:     %lld\n", threadObjectId);
	fprintf(stdout, "stackTraceSerialNumber:      %d\n", stackTraceSerialNumber);
	fprintf(stdout, "threadNameStringId:     %lld\n", threadNameStringId);
	fprintf(stdout, "threadGroupNameId:     %lld\n", threadGroupNameId);
	fprintf(stdout, "threadParentGroupNameId:     %lld\n", threadParentGroupNameId);

	return 0;
}

/**
u4 thread serial number
*/
int processTagEndThread(TagInfo tagInfo) {
	fprintf(stdout, "TAG_END_THREAD\n");
	int totalRequiredBytes = 1 * 4;
	if (tagInfo.dataLength != totalRequiredBytes) {
		fprintf(stderr, "TAG_END_THREAD required %d bytes but we only got %d.\n", totalRequiredBytes, tagInfo.dataLength);
	}

	unsigned int threadSerialNumber;
	if (readBigEndianStreamToInt(tagInfo.stream, &threadSerialNumber) != 0) {
		fprintf(stderr, "unable to read u4 threadSerialNumber for TAG_END_THREAD\n");
		return -1;
	}

	fprintf(stdout, "threadSerialNumber:      %d\n", threadSerialNumber);

	return 0;
}




/**
* see processTagHeap
*/
int processTagHeapDump(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_HEAP_DUMP\n");
	return processTagHeap(f, dataLength);
}

/**
* see processTagHeap
*/
int processTagHeapSegment(FILE * f, int dataLength) {
	fprintf(stdout, "TAG_HEAP_DUMP_SEGMENT\n");
	return processTagHeap(f, dataLength);
}

/**
* Terminates a series of HEAP DUMP SEGMENTS.  Concatenation of HEAP DUMP SEGMENTS equals a HEAP DUMP.
*/
int processTagHeapDumpEnd(TagInfo tagInfo) {
	fprintf(stdout, "TAG_HEAP_DUMP_END\n");
	int totalRequiredBytes = 0;
	if (tagInfo.dataLength != totalRequiredBytes) {
		fprintf(stderr, "TAG_HEAP_DUMP_END required %d bytes but we got %d.\n", totalRequiredBytes, tagInfo.dataLength);
	}
	return 0;
}

/**
u4 total number of samples
u4 number of traces that follow:
  for each
    u4 number of samples
    u4 stack trace serial number
*/
int processTagCpuSamples(TagInfo tagInfo) {
	fprintf(stdout, "TAG_CPU_SAMPLES\n");
	int totalRequiredBytes = 2*4;
	if (tagInfo.dataLength >= totalRequiredBytes) {
		fprintf(stderr, "TAG_CPU_SAMPLES required %d bytes but we got %d.\n", totalRequiredBytes, tagInfo.dataLength);
	}
	
	unsigned int numberOfSamples;
	if (readBigEndianStreamToInt(tagInfo.stream, &numberOfSamples) != 0) {
		fprintf(stderr, "unable to read u4 numberOfSamples for TAG_CPU_SAMPLES\n");
		return -1;
	}
	unsigned int numberOfTraces;
	if (readBigEndianStreamToInt(tagInfo.stream, &numberOfTraces) != 0) {
		fprintf(stderr, "unable to read u4 numberOfTraces for TAG_CPU_SAMPLES\n");
		return -1;
	}

	int bytesLeft = tagInfo.dataLength - totalRequiredBytes;
	if (bytesLeft != 4 * 2 * numberOfTraces) {
		fprintf(stderr, "TAG_CPU_SAMPLES required %d bytes for %d numberOfTraces but we got %d.\n", 4 * 2 * numberOfTraces, numberOfTraces, bytesLeft);
		return -1;
	}

	fprintf(stdout, "numberOfSamples:      %d\n", numberOfSamples);

	for (unsigned int i = 0; i < numberOfTraces; i++) {
		int errorCode = processTagCpuSampleTrace(tagInfo);
		if (errorCode != 0) {
			fprintf(stderr, "could not get %d th trace for TAG_CPU_SAMPLES\n", i);
			return errorCode;
		}
	}

	return 0;
}

/*
u4 number of samples
u4 stack trace serial number
*/
int processTagCpuSampleTrace(TagInfo tagInfo) {
	unsigned int numberOfSamples;
	if (readBigEndianStreamToInt(tagInfo.stream, &numberOfSamples) != 0) {
		fprintf(stderr, "unable to read u4 numberOfSamples for TAG_CPU_SAMPLES\n");
		return -1;
	}
	unsigned int stackTraceSerialNumber;
	if (readBigEndianStreamToInt(tagInfo.stream, &stackTraceSerialNumber) != 0) {
		fprintf(stderr, "unable to read u4 stackTraceSerialNumber for TAG_CPU_SAMPLES\n");
		return -1;
	}

	fprintf(stdout, "numberOfSamples:        %d\n", numberOfSamples);
	fprintf(stdout, "stackTraceSerialNumber: %d\n", stackTraceSerialNumber);


	return 0;
}


/**
u4 Bit mask flags:
0x1 alloc traces on/off
0x2 cpu sampling on/off

u2 stack trace depth
*/
int processTagControlSettings(TagInfo tagInfo) {
	fprintf(stdout, "TAG_CONTROL_SETTINGS\n");
	int totalRequiredBytes = 4 + 2;
	if (tagInfo.dataLength != totalRequiredBytes) {
		fprintf(stderr, "TAG_CONTROL_SETTINGS required %d bytes but we got %d.\n", totalRequiredBytes, tagInfo.dataLength);
	}

	unsigned int controlMask;
	if (readBigEndianStreamToInt(tagInfo.stream, &controlMask) != 0) {
		fprintf(stderr, "unable to read u4 controlMask for TAG_CONTROL_SETTINGS\n");
		return -1;
	}
	unsigned int stackTraceDepth;
	if (readTwoByteBigEndianStreamToInt(tagInfo.stream, &stackTraceDepth) != 0) {
		fprintf(stderr, "unable to read u4 stackTraceDepth for TAG_CONTROL_SETTINGS\n");
		return -1;
	}


	fprintf(stdout, "controlMask:      %d\n", controlMask);
	fprintf(stdout, "stackTraceDepth:      %d\n", stackTraceDepth);

	return 0;
}