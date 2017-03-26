#include "ProcessTagHeap.h"
#include "StreamUtil.h"
#include <stdlib.h>




int processHeapRootUnknown(FILE * f, int dataLength);
int processHeapRootJniGlobal(FILE * f, int dataLength);
int processHeapRootJniLocal(FILE * f, int dataLength);
int processHeapRootJavaFrame(FILE * f, int dataLength);
int processHeapRootNativeStack(FILE * f, int dataLength);
int processHeapRootStickyClass(FILE * f, int dataLength);
int processHeapRootThreadBlock(FILE * f, int dataLength);
int processHeapRootMonitorUsed(FILE * f, int dataLength);
int processHeapRootThreadObject(FILE * f, int dataLength);
int processHeapRootThreadObject(FILE * f, int dataLength);
int processHeapClassDump(FILE * f, int dataLength);
int processHeapInstanceDump(FILE * f, int dataLength);
int processHeapObjectArrayDump(FILE * f, int dataLength);
int processHeapPrimitiveArrayDump(FILE * f, int dataLength);

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
int processTagHeap(FILE * f, int dataLength) {
	switch (tagType) {
		case HEAP_ROOT_UNKNOWN:
			processHeapRootUnknown(f, dataLength);
		case HEAP_ROOT_JNI_GLOBAL:
			processHeapRootJniGlobal(f, dataLength);
		case HEAP_ROOT_JNI_LOCAL:
			processHeapRootJniLocal(f, dataLength);
		case HEAP_ROOT_JAVA_FRAME:
			processHeapRootJavaFrame(f, dataLength);
		case HEAP_ROOT_NATIVE_STACK:
			processHeapRootNativeStack(f, dataLength);
		case HEAP_ROOT_STICKY_CLASS:
			processHeapRootStickyClass(f, dataLength);
		case HEAP_ROOT_THREAD_BLOCK:
			processHeapRootThreadBlock(f, dataLength);
		case HEAP_ROOT_MONITOR_USED:
			processHeapRootMonitorUsed(f, dataLength);
		case HEAP_ROOT_THREAD_OBJECT:
			processHeapRootThreadObject(f, dataLength);
		case HEAP_ROOT_THREAD_OBJECT:
			processHeapRootThreadObject(f, dataLength);
		case HEAP_CLASS_DUMP:
			processHeapClassDump(f, dataLength);
		case HEAP_INSTANCE_DUMP:
			processHeapInstanceDump(f, dataLength);
		case HEAP_OBJECT_ARRAY_DUMP:
			processHeapObjectArrayDump(f, dataLength);
		case HEAP_PRIMITIVE_ARRAY_DUMP:
			processHeapPrimitiveArrayDump(f, dataLength);
		case HEAP_DUMP_END:
		default:
			fprintf(stdout, "tag not recognized or implemented: %d", tagType);
			return iterateThroughStream(tagInfo.stream, tagInfo.dataLength);
	}

	// TODO
	return iterateThroughStream(f, dataLength);
}



/*
HEAP_ROOT_UNKNOWN
ID object ID
*/

int processHeapRootUnknown(FILE * f, int dataLength) {
	return iterateThroughStream(f, dataLength);
}

/*
HEAP_ROOT_JNI_GLOBAL
ID object ID
ID JNI global ref ID
*/
int processHeapRootJniGlobal(FILE * f, int dataLength){
	// TODO
	return iterateThroughStream(f, dataLength);
}

/*
HEAP_ROOT_JNI_LOCAL
ID object ID
u4 thread serial number
u4 frame number in stack trace(-1 for empty)
*/
int processHeapRootJniLocal(FILE * f, int dataLength){
	// TODO
	return iterateThroughStream(f, dataLength);
}

/*
HEAP_ROOT_JAVA_FRAME
ID object ID
u4 thread serial number
u4 frame number in stack trace(-1 for empty)
*/
int processHeapRootJavaFrame(FILE * f, int dataLength){
	// TODO
	return iterateThroughStream(f, dataLength);
}

/*
HEAP_ROOT_NATIVE_STACK
ID object ID
u4 thread serial number
*/
int processHeapRootNativeStack(FILE * f, int dataLength){
	// TODO
	return iterateThroughStream(f, dataLength);
}

/*
HEAP_ROOT_STICKY_CLASS
ID object ID
*/
int processHeapRootStickyClass(FILE * f, int dataLength){
	// TODO
	return iterateThroughStream(f, dataLength);
}

/*
HEAP_ROOT_THREAD_BLOCK
ID object ID
u4 thread serial number
*/
int processHeapRootThreadBlock(FILE * f, int dataLength){
	// TODO
	return iterateThroughStream(f, dataLength);
}

/*
HEAP_ROOT_MONITOR_USED
ID object ID
*/
int processHeapRootMonitorUsed(FILE * f, int dataLength){
	// TODO
	return iterateThroughStream(f, dataLength);
}

/*
HEAP_ROOT_THREAD_OBJECT
ID thread object id
u4 thread serial number
u4 stack trace serial number
*/
int processHeapRootThreadObject(FILE * f, int dataLength){
	// TODO
	return iterateThroughStream(f, dataLength);
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
int processHeapClassDump(FILE * f, int dataLength){
	// TODO
	return iterateThroughStream(f, dataLength);
}

/*
HEAP_INSTANCE_DUMP
ID object ID
u4 stack trace serial number
ID class object ID
u4 number of bytes that follow
	[value] * instance field values(this class, followed by super class, etc)
*/
int processHeapInstanceDump(FILE * f, int dataLength){
	// TODO
	return iterateThroughStream(f, dataLength);
}

/*
HEAP_OBJECT_ARRAY_DUMP
ID array object ID
u4 stack trace serial number
u4 number of elements
ID array class object ID
[ID] * elements
*/
int processHeapObjectArrayDump(FILE * f, int dataLength){
	// TODO
	return iterateThroughStream(f, dataLength);
}

/*
HEAP_PRIMITIVE_ARRAY_DUMP
ID array object ID
u4 stack trace serial number
u4 number of elements
u1 element type(See Basic Type)
[u1] * elements(packed array)
*/
int processHeapPrimitiveArrayDump(FILE * f, int dataLength){
	// TODO
	return iterateThroughStream(f, dataLength);
}


