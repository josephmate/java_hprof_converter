#include "ProcessTagHeap.h"
#include "StreamUtil.h"
#include <stdlib.h>




int processHeapRootUnknown(TagInfo * tagInfo);
int processHeapRootJniGlobal(TagInfo * tagInfo);
int processHeapRootJniLocal(TagInfo * tagInfo);
int processHeapRootJavaFrame(TagInfo * tagInfo);
int processHeapRootNativeStack(TagInfo * tagInfo);
int processHeapRootStickyClass(TagInfo * tagInfo);
int processHeapRootThreadBlock(TagInfo * tagInfo);
int processHeapRootMonitorUsed(TagInfo * tagInfo);
int processHeapRootThreadObject(TagInfo * tagInfo);
int processHeapRootThreadObject(TagInfo * tagInfo);
int processHeapClassDump(TagInfo * tagInfo);
int processHeapInstanceDump(TagInfo * tagInfo);
int processHeapObjectArrayDump(TagInfo * tagInfo);
int processHeapPrimitiveArrayDump(TagInfo * tagInfo);

/*
BASIC TYPES
2 object
4 boolean
5 char
6 float
7 double
8 byte
9 short
10 int
11 long
*/

#define HEAP_ROOT_UNKNOWN            0xFF
#define HEAP_ROOT_JNI_GLOBAL         0x01
#define HEAP_ROOT_JNI_LOCAL          0x02
#define HEAP_ROOT_JAVA_FRAME         0x03
#define HEAP_ROOT_NATIVE_STACK       0x04
#define HEAP_ROOT_STICKY_CLASS       0x05
#define HEAP_ROOT_THREAD_BLOCK       0x06
#define HEAP_ROOT_MONITOR_USED       0x07
#define HEAP_ROOT_THREAD_OBJECT      0x08
#define HEAP_ROOT_THREAD_OBJECT      0x08
#define HEAP_CLASS_DUMP              0x20
#define HEAP_INSTANCE_DUMP           0x21
#define HEAP_OBJECT_ARRAY_DUMP       0x22
#define HEAP_PRIMITIVE_ARRAY_DUMP    0x23
#define HEAP_DUMP_END                0x2C
int processTagHeap(TagInfo tagInfo) {
	unsigned int tagType;

	while (tagInfo.dataLength > 0) {
		int errCode = readByteToInt(tagInfo.stream, &tagType);
		if (errCode != 0) {
			fprintf(stderr, "could not read heap tag type. reached end of stream too early\n");
			return errCode;
		}
		fprintf(stdout, "heap tag type: %d \n", tagType);
		tagInfo.dataLength = tagInfo.dataLength - 1;
		switch (tagType) {
		case HEAP_ROOT_UNKNOWN:
			errCode = processHeapRootUnknown(&tagInfo);
			if (errCode != 0) {
				return errCode;
			}
			break;
		case HEAP_ROOT_JNI_GLOBAL:
			errCode = processHeapRootJniGlobal(&tagInfo);
			if (errCode != 0) {
				return errCode;
			}
			break;
		case HEAP_ROOT_JNI_LOCAL:
			errCode = processHeapRootJniLocal(&tagInfo);
			if (errCode != 0) {
				return errCode;
			}
			break;
		case HEAP_ROOT_JAVA_FRAME:
			errCode = processHeapRootJavaFrame(&tagInfo);
			if (errCode != 0) {
				return errCode;
			}
			break;
		case HEAP_ROOT_NATIVE_STACK:
			errCode = processHeapRootNativeStack(&tagInfo);
			if (errCode != 0) {
				return errCode;
			}
			break;
		case HEAP_ROOT_STICKY_CLASS:
			errCode = processHeapRootStickyClass(&tagInfo);
			if (errCode != 0) {
				return errCode;
			}
			break;
		case HEAP_ROOT_THREAD_BLOCK:
			errCode = processHeapRootThreadBlock(&tagInfo);
			if (errCode != 0) {
				return errCode;
			}
			break;
		case HEAP_ROOT_MONITOR_USED:
			errCode = processHeapRootMonitorUsed(&tagInfo);
			if (errCode != 0) {
				return errCode;
			}
			break;
		case HEAP_ROOT_THREAD_OBJECT:
			errCode = processHeapRootThreadObject(&tagInfo);
			if (errCode != 0) {
				return errCode;
			}
			break;
		case HEAP_CLASS_DUMP:
			errCode = processHeapClassDump(&tagInfo);
			if (errCode != 0) {
				return errCode;
			}
			break;
		case HEAP_INSTANCE_DUMP:
			errCode = processHeapInstanceDump(&tagInfo);
			if (errCode != 0) {
				return errCode;
			}
			break;
		case HEAP_OBJECT_ARRAY_DUMP:
			errCode = processHeapObjectArrayDump(&tagInfo);
			if (errCode != 0) {
				return errCode;
			}
			break;
		case HEAP_PRIMITIVE_ARRAY_DUMP:
			errCode = processHeapPrimitiveArrayDump(&tagInfo);
			if (errCode != 0) {
				return errCode;
			}
			break;
		case HEAP_DUMP_END:
			return 0;
		default:
			fprintf(stdout, "tag not recognized or implemented: %d", tagType);
			return iterateThroughStream(tagInfo.stream, tagInfo.dataLength);
		}
	}

	fprintf(stderr, "was expecting tag %d but never found\n", HEAP_DUMP_END);
	return -1;
}



/*
HEAP_ROOT_UNKNOWN
ID object ID
*/

int processHeapRootUnknown(TagInfo * tagInfo) {
	// TODO
	fprintf(stdout, "HEAP_ROOT_UNKNOWN\n");
	int errCode = iterateThroughStream(tagInfo->stream, tagInfo->dataLength);
	tagInfo->dataLength = 0;
	return errCode;
}

/*
HEAP_ROOT_JNI_GLOBAL
ID object ID
ID JNI global ref ID
*/
int processHeapRootJniGlobal(TagInfo * tagInfo) {
	// TODO
	fprintf(stdout, "HEAP_ROOT_JNI_GLOBAL\n");
	int errCode = iterateThroughStream(tagInfo->stream, tagInfo->dataLength);
	tagInfo->dataLength = 0;
	return errCode;
}

/*
HEAP_ROOT_JNI_LOCAL
ID object ID
u4 thread serial number
u4 frame number in stack trace(-1 for empty)
*/
int processHeapRootJniLocal(TagInfo * tagInfo) {
	// TODO
	fprintf(stdout, "HEAP_ROOT_JNI_LOCAL\n");
	int errCode = iterateThroughStream(tagInfo->stream, tagInfo->dataLength);
	tagInfo->dataLength = 0;
	return errCode;
}

/*
HEAP_ROOT_JAVA_FRAME
ID object ID
u4 thread serial number
u4 frame number in stack trace(-1 for empty)
*/
int processHeapRootJavaFrame(TagInfo * tagInfo) {
	// TODO
	fprintf(stdout, "HEAP_ROOT_JAVA_FRAME\n");
	int errCode = iterateThroughStream(tagInfo->stream, tagInfo->dataLength);
	tagInfo->dataLength = 0;
	return errCode;
}

/*
HEAP_ROOT_NATIVE_STACK
ID object ID
u4 thread serial number
*/
int processHeapRootNativeStack(TagInfo * tagInfo) {
	// TODO
	fprintf(stdout, "HEAP_ROOT_NATIVE_STACK\n");
	int errCode = iterateThroughStream(tagInfo->stream, tagInfo->dataLength);
	tagInfo->dataLength = 0;
	return errCode;
}

/*
HEAP_ROOT_STICKY_CLASS
ID object ID
*/
int processHeapRootStickyClass(TagInfo * tagInfo) {
	// TODO
	fprintf(stdout, "HEAP_ROOT_STICKY_CLASS\n");
	int errCode = iterateThroughStream(tagInfo->stream, tagInfo->dataLength);
	tagInfo->dataLength = 0;
	return errCode;
}

/*
HEAP_ROOT_THREAD_BLOCK
ID object ID
u4 thread serial number
*/
int processHeapRootThreadBlock(TagInfo * tagInfo) {
	// TODO
	fprintf(stdout, "HEAP_ROOT_THREAD_BLOCK\n");
	int errCode = iterateThroughStream(tagInfo->stream, tagInfo->dataLength);
	tagInfo->dataLength = 0;
	return errCode;
}

/*
HEAP_ROOT_MONITOR_USED
ID object ID
*/
int processHeapRootMonitorUsed(TagInfo * tagInfo) {
	// TODO
	fprintf(stdout, "HEAP_ROOT_MONITOR_USED\n");
	int errCode = iterateThroughStream(tagInfo->stream, tagInfo->dataLength);
	tagInfo->dataLength = 0;
	return errCode;
}

/*
HEAP_ROOT_THREAD_OBJECT
ID thread object id
u4 thread serial number
u4 stack trace serial number
*/
int processHeapRootThreadObject(TagInfo * tagInfo) {
	// TODO
	fprintf(stdout, "HEAP_ROOT_THREAD_OBJECT\n");
	int errCode = iterateThroughStream(tagInfo->stream, tagInfo->dataLength);
	tagInfo->dataLength = 0;
	return errCode;
}

/*
HEAP_CLASS_DUMP
ID class object ID
u4 stack trace serial number
ID super class object ID
ID class loader object ID
ID signers object ID
ID protection domain object ID
ID reserved
ID reserved
u4 instance size(in bytes)
u2 size of constant pool and number of records that follow :
u2 constant pool index
u1 type of entry : (See Basic Type)
value value of entry(u1, u2, u4, or u8 based on type of entry)

u2 Number of static fields :
ID static field name string ID
u1 type of field : (See Basic Type)
value value of entry(u1, u2, u4, or u8 based on type of field)

u2 Number of instance fields(not including super class's)
ID field name string ID
u1 type of field : (See Basic Type)
*/
int processHeapClassDump(TagInfo * tagInfo) {
	// TODO
	fprintf(stdout, "HEAP_CLASS_DUMP\n");
	int errCode = iterateThroughStream(tagInfo->stream, tagInfo->dataLength);
	tagInfo->dataLength = 0;
	return errCode;
}

/*
HEAP_INSTANCE_DUMP
ID object ID
u4 stack trace serial number
ID class object ID
u4 number of bytes that follow
	[value] * instance field values(this class, followed by super class, etc)
*/
int processHeapInstanceDump(TagInfo * tagInfo) {
	// TODO
	fprintf(stdout, "HEAP_INSTANCE_DUMP\n");
	int errCode = iterateThroughStream(tagInfo->stream, tagInfo->dataLength);
	tagInfo->dataLength = 0;
	return errCode;
}

/*
HEAP_OBJECT_ARRAY_DUMP
ID array object ID
u4 stack trace serial number
u4 number of elements
ID array class object ID
[ID] * elements
*/
int processHeapObjectArrayDump(TagInfo * tagInfo) {
	// TODO
	fprintf(stdout, "HEAP_OBJECT_ARRAY_DUMP\n");
	int errCode = iterateThroughStream(tagInfo->stream, tagInfo->dataLength);
	tagInfo->dataLength = 0;
	return errCode;
}

/*
HEAP_PRIMITIVE_ARRAY_DUMP
ID array object ID
u4 stack trace serial number
u4 number of elements
u1 element type(See Basic Type)
[u1] * elements(packed array)
*/
int processHeapPrimitiveArrayDump(TagInfo * tagInfo) {
	// TODO
	fprintf(stdout, "HEAP_PRIMITIVE_ARRAY_DUMP\n");
	int errCode = iterateThroughStream(tagInfo->stream, tagInfo->dataLength);
	tagInfo->dataLength = 0;
	return errCode;
}


