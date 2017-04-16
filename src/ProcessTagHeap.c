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
int processConstantPoolRecord(TagInfo * tagInfo, unsigned int entry);
int processStaticFieldRecord(TagInfo * tagInfo, unsigned int entry);
int processBasicTypeValue(TagInfo * tagInfo, unsigned int entry, const char * source);
int processInstanceFieldRecord(TagInfo * tagInfo, unsigned int entry);
int processHeapInstanceDump(TagInfo * tagInfo);
int processHeapObjectArrayDump(TagInfo * tagInfo);
int processHeapPrimitiveArrayDump(TagInfo * tagInfo);
int processHeapPrimitiveArrayRecord(TagInfo * tagInfo, unsigned int typeOfEntry, unsigned int entry, const char * source);

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
#define BASIC_TYPE_OBJECT 2
#define BASIC_TYPE_BOOLEAN 4
#define BASIC_TYPE_CHAR 5
#define BASIC_TYPE_FLOAT 6
#define BASIC_TYPE_DOUBLE 7
#define BASIC_TYPE_BYTE 8
#define BASIC_TYPE_SHORT 9
#define BASIC_TYPE_INT 10
#define BASIC_TYPE_LONG 11


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
	//long long bytesLastTime = tagInfo.dataLength;

	while (tagInfo.dataLength > 0) {
		int errCode = readByteToInt(tagInfo.stream, &tagType);
		if (errCode != 0) {
			fprintf(stderr, "could not read heap tag type. reached end of stream too early. datalength:%lld\n", tagInfo.dataLength);
			return errCode;
		}
		tagInfo.dataLength = tagInfo.dataLength - 1;
		//fprintf(stdout, "heap tag type: %d bytes left %lld, usedSinceLast%lld, usedSinceLastIgnoreTag:%lld \n", tagType, tagInfo.dataLength, bytesLastTime - tagInfo.dataLength, bytesLastTime - tagInfo.dataLength - 1);
		//bytesLastTime = tagInfo.dataLength;
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
			fprintf(stdout, "HEAP_DUMP_END\n");
			return 0;
		default:
			fprintf(stdout, "HEAP TAG not recognized or implemented: %d\n", tagType);
			return iterateThroughStream(tagInfo.stream, tagInfo.dataLength);
		}
	}

	fprintf(stdout, "HEAP_DUMP_NO_MORE_BYTES\n");
	//fprintf(stderr, "was expecting tag %d but never found\n", HEAP_DUMP_END);
	return 0;
}



/*
HEAP_ROOT_UNKNOWN
ID object ID
*/

int processHeapRootUnknown(TagInfo * tagInfo) {
	unsigned long long id;
	int errCode = getId(tagInfo->stream, tagInfo->idSize, &id);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain object id of HEAP_ROOT_UNKNOWN\n");
		return errCode;
	}
	fprintf(stdout, "HEAP_ROOT_UNKNOWN %lld\n", id);
	tagInfo->dataLength = tagInfo->dataLength 
		- tagInfo->idSize;
	return errCode;
}

/*
HEAP_ROOT_JNI_GLOBAL
ID object ID
ID JNI global ref ID
*/
int processHeapRootJniGlobal(TagInfo * tagInfo) {
	unsigned long long objId;
	unsigned long long jniGlobalRefId;

	int errCode = getId(tagInfo->stream, tagInfo->idSize, &objId);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain objId of HEAP_ROOT_JNI_GLOBAL\n");
		return errCode;
	}

	errCode = getId(tagInfo->stream, tagInfo->idSize, &jniGlobalRefId);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain jniGlobalRefId of HEAP_ROOT_JNI_GLOBAL\n");
		return errCode;
	}

	fprintf(stdout, "HEAP_ROOT_JNI_GLOBAL objId:%lld jniGlobalRefId:%lld\n", objId, jniGlobalRefId);

	tagInfo->dataLength = tagInfo->dataLength
		- 2 * tagInfo->idSize;
	return errCode;
}

/*
HEAP_ROOT_JNI_LOCAL
ID object ID
u4 thread serial number
u4 frame number in stack trace(-1 for empty)
*/
int processHeapRootJniLocal(TagInfo * tagInfo) {
	unsigned long long objId;
	unsigned int threadSerialNum;
	unsigned int frameNumber;

	int errCode = getId(tagInfo->stream, tagInfo->idSize, &objId);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain objId of HEAP_ROOT_JNI_LOCAL\n");
		return errCode;
	}

	errCode = readBigEndianStreamToInt(tagInfo->stream, &threadSerialNum);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain threadSerialNum of HEAP_ROOT_JNI_LOCAL\n");
		return errCode;
	}

	errCode = readBigEndianStreamToInt(tagInfo->stream, &frameNumber);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain frameNumber of HEAP_ROOT_JNI_LOCAL\n");
		return errCode;
	}

	fprintf(stdout, "HEAP_ROOT_JNI_LOCAL objId:%lld, threadSerialNum:%d, frameNumber:%d\n", objId, threadSerialNum, frameNumber);

	tagInfo->dataLength = tagInfo->dataLength
		- 1 * tagInfo->idSize
		- 2 * 4;
	return 0;
}

/*
HEAP_ROOT_JAVA_FRAME
ID object ID
u4 thread serial number
u4 frame number in stack trace(-1 for empty)
*/
int processHeapRootJavaFrame(TagInfo * tagInfo) {
	unsigned long long objId;
	unsigned int threadSerialNum;
	unsigned int frameNumber;

	int errCode = getId(tagInfo->stream, tagInfo->idSize, &objId);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain objId of HEAP_ROOT_JAVA_FRAME\n");
		return errCode;
	}

	errCode = readBigEndianStreamToInt(tagInfo->stream, &threadSerialNum);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain threadSerialNum of HEAP_ROOT_JAVA_FRAME\n");
		return errCode;
	}

	errCode = readBigEndianStreamToInt(tagInfo->stream, &frameNumber);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain frameNumber of HEAP_ROOT_JAVA_FRAME\n");
		return errCode;
	}

	fprintf(stdout, "HEAP_ROOT_JAVA_FRAME objId:%lld, threadSerialNum:%d, frameNumber:%d\n", objId, threadSerialNum, frameNumber);

	tagInfo->dataLength = tagInfo->dataLength
		- 1 * tagInfo->idSize
		- 2 * 4;
	return 0;
}

/*
HEAP_ROOT_NATIVE_STACK
ID object ID
u4 thread serial number
*/
int processHeapRootNativeStack(TagInfo * tagInfo) {
	unsigned long long objId;
	unsigned int threadSerialNum;

	int errCode = getId(tagInfo->stream, tagInfo->idSize, &objId);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain objId of HEAP_ROOT_NATIVE_STACK\n");
		return errCode;
	}

	errCode = readBigEndianStreamToInt(tagInfo->stream, &threadSerialNum);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain threadSerialNum of HEAP_ROOT_NATIVE_STACK\n");
		return errCode;
	}

	fprintf(stdout, "HEAP_ROOT_NATIVE_STACK objId:%lld, threadSerialNum:%d\n", objId, threadSerialNum);

	tagInfo->dataLength = tagInfo->dataLength 
		- 1 * tagInfo->idSize
		- 1 * 4;
	return 0;
}

/*
HEAP_ROOT_STICKY_CLASS
ID object ID
*/
int processHeapRootStickyClass(TagInfo * tagInfo) {
	unsigned long long id;
	int errCode = getId(tagInfo->stream, tagInfo->idSize, &id);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain object id of HEAP_ROOT_STICKY_CLASS\n");
		return errCode;
	}
	fprintf(stdout, "HEAP_ROOT_STICKY_CLASS objId:%lld\n", id);
	tagInfo->dataLength = tagInfo->dataLength
		- 1 * tagInfo->idSize;
	return errCode;
}

/*
HEAP_ROOT_THREAD_BLOCK
ID object ID
u4 thread serial number
*/
int processHeapRootThreadBlock(TagInfo * tagInfo) {
	unsigned long long objId;
	unsigned int threadSerialNum;

	int errCode = getId(tagInfo->stream, tagInfo->idSize, &objId);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain objId of HEAP_ROOT_THREAD_BLOCK\n");
		return errCode;
	}

	errCode = readBigEndianStreamToInt(tagInfo->stream, &threadSerialNum);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain threadSerialNum of HEAP_ROOT_THREAD_BLOCK\n");
		return errCode;
	}

	fprintf(stdout, "HEAP_ROOT_THREAD_BLOCK objId:%lld, threadSerialNum:%d\n", objId, threadSerialNum);

	tagInfo->dataLength = tagInfo->dataLength
		- 1 * tagInfo->idSize
		- 1 * 4;
	return 0;
}

/*
HEAP_ROOT_MONITOR_USED
ID object ID
*/
int processHeapRootMonitorUsed(TagInfo * tagInfo) {
	unsigned long long id;
	int errCode = getId(tagInfo->stream, tagInfo->idSize, &id);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain object id of HEAP_ROOT_MONITOR_USED\n");
		return errCode;
	}
	fprintf(stdout, "HEAP_ROOT_MONITOR_USED objId:%lld\n", id);
	tagInfo->dataLength = tagInfo->dataLength
		- 1 * tagInfo->idSize;
	return errCode;
}

/*
HEAP_ROOT_THREAD_OBJECT
ID thread object id
u4 thread serial number
u4 stack trace serial number
*/
int processHeapRootThreadObject(TagInfo * tagInfo) {
	fprintf(stdout, "HEAP_ROOT_THREAD_OBJECT\n");
	unsigned long long objId;
	unsigned int threadSerialNum;
	unsigned int frameNumber;

	int errCode = getId(tagInfo->stream, tagInfo->idSize, &objId);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain objId of HEAP_ROOT_THREAD_OBJECT\n");
		return errCode;
	}

	errCode = readBigEndianStreamToInt(tagInfo->stream, &threadSerialNum);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain threadSerialNum of HEAP_ROOT_THREAD_OBJECT\n");
		return errCode;
	}

	errCode = readBigEndianStreamToInt(tagInfo->stream, &frameNumber);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain frameNumber of HEAP_ROOT_THREAD_OBJECT\n");
		return errCode;
	}

	fprintf(stdout, "HEAP_ROOT_THREAD_OBJECT objId:%lld, threadSerialNum:%d, frameNumber:%d\n", objId, threadSerialNum, frameNumber);

	tagInfo->dataLength = tagInfo->dataLength
		- 1 * tagInfo->idSize
		- 2 * 4;
	return 0;
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
	unsigned long long classObjId;
	unsigned int stackTraceSerialNumber;
	unsigned long long superClassObjId;
	unsigned long long classLoaderObjectId;
	unsigned long long signersObjectId;
	unsigned long long protectionDomainObjectId;
	unsigned long long reserved1;
	unsigned long long reserved2;
	unsigned int instanceSize;
	unsigned int sizeOfConstantPool;



	int errCode = getId(tagInfo->stream, tagInfo->idSize, &classObjId);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain classObjId of HEAP_CLASS_DUMP\n");
		return errCode;
	}
	errCode = readBigEndianStreamToInt(tagInfo->stream, &stackTraceSerialNumber);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain stackTraceSerialNumber of HEAP_CLASS_DUMP\n");
		return errCode;
	}
	errCode = getId(tagInfo->stream, tagInfo->idSize, &superClassObjId);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain superClassObjId of HEAP_CLASS_DUMP\n");
		return errCode;
	}

	errCode = getId(tagInfo->stream, tagInfo->idSize, &classLoaderObjectId);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain classLoaderObjectId of HEAP_CLASS_DUMP\n");
		return errCode;
	}
	errCode = getId(tagInfo->stream, tagInfo->idSize, &signersObjectId);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain signersObjectId of HEAP_CLASS_DUMP\n");
		return errCode;
	}
	errCode = getId(tagInfo->stream, tagInfo->idSize, &protectionDomainObjectId);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain protectionDomainObjectId of HEAP_CLASS_DUMP\n");
		return errCode;
	}
	errCode = getId(tagInfo->stream, tagInfo->idSize, &reserved1);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain reserved1 of HEAP_CLASS_DUMP\n");
		return errCode;
	}
	errCode = getId(tagInfo->stream, tagInfo->idSize, &reserved2);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain reserved2 of HEAP_CLASS_DUMP\n");
		return errCode;
	}
	errCode = readBigEndianStreamToInt(tagInfo->stream, &instanceSize);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain instanceSize of HEAP_CLASS_DUMP\n");
		return errCode;
	}
	errCode = readTwoByteBigEndianStreamToInt(tagInfo->stream, &sizeOfConstantPool);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain sizeOfConstantPool of HEAP_CLASS_DUMP\n");
		return errCode;
	}
	fprintf(stdout,
		"HEAP_CLASS_DUMP classObjId:%lld, stackTraceSerialNumber:%d, superClassObjId:%lld, classLoaderObjectId:%lld, signersObjectId:%lld, protectionDomainObjectId:%lld, reserved1:%lld, reserved2:%lld, instanceSize:%d, sizeOfConstantPool:%d\n",
		classObjId, stackTraceSerialNumber, superClassObjId, classLoaderObjectId, signersObjectId, protectionDomainObjectId, reserved1, reserved2, instanceSize, sizeOfConstantPool
	);
	// 7 ID
	// 2 u4
	// 1 u2
	tagInfo->dataLength = tagInfo->dataLength
		- 7 * tagInfo->idSize
		- 2 * 4
		- 1 * 2;
	// iterate over constant pool
	for (unsigned int i = 0; i < sizeOfConstantPool; i++) {
		errCode = processConstantPoolRecord(tagInfo, i);
		if (errCode != 0) {
			return errCode;
		}
	}


	unsigned int numStaticFieldRecords;
	errCode = readTwoByteBigEndianStreamToInt(tagInfo->stream, &numStaticFieldRecords);
	tagInfo->dataLength = tagInfo->dataLength - 2;
	fprintf(stdout, "\tnumStaticFieldRecords:%d\n", numStaticFieldRecords);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain numStaticFieldRecords of HEAP_CLASS_DUMP\n");
		return errCode;
	}
	// iterate over static field records
	for (unsigned int i = 0; i < numStaticFieldRecords; i++) {
		errCode = processStaticFieldRecord(tagInfo, i);
		if (errCode != 0) {
			return errCode;
		}
	}
	

	unsigned int numInstanceFields;
	errCode = readTwoByteBigEndianStreamToInt(tagInfo->stream, &numInstanceFields);
	tagInfo->dataLength = tagInfo->dataLength - 2;
	fprintf(stdout, "\tnumInstanceFields:%d\n", numInstanceFields);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain numInstanceFields of HEAP_CLASS_DUMP\n");
		return errCode;
	}
	// iterate over instance field records
	for (unsigned int i = 0; i < numInstanceFields; i++) {
		errCode = processInstanceFieldRecord(tagInfo, i);
		if (errCode != 0) {
			return errCode;
		}
	}

	return 0;
}

/*
	u2 constant pool index
	u1 type of entry : (See Basic Type)
	value value of entry(u1, u2, u4, or u8 based on type of entry)
*/
int processConstantPoolRecord(TagInfo * tagInfo, unsigned int entry) {
	unsigned int constantPoolIndex;

	int errCode = readTwoByteBigEndianStreamToInt(tagInfo->stream, &constantPoolIndex);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain constantPoolIndex entry %d of processConstantPoolRecord\n", entry);
		return errCode;
	}

	fprintf(stdout, "\tconstantPoolIndex:%d, ", constantPoolIndex);
	tagInfo->dataLength = tagInfo->dataLength - 2;

	errCode = processBasicTypeValue(tagInfo, entry, "processConstantPoolRecord");
	return errCode;
}

/*
ID static field name string ID
u1 type of field : (See Basic Type)
value value of entry(u1, u2, u4, or u8 based on type of field)
*/
int processStaticFieldRecord(TagInfo * tagInfo, unsigned int entry) {
	unsigned long long staticFieldNameId;

	int errCode = getId(tagInfo->stream, tagInfo->idSize, &staticFieldNameId);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain staticFieldNameId entry %d of processStaticFieldRecord\n", entry);
		return errCode;
	}

	fprintf(stdout, "\tstaticFieldNameId:%lld, ", staticFieldNameId);
	tagInfo->dataLength = tagInfo->dataLength - tagInfo->idSize;

	errCode = processBasicTypeValue(tagInfo, entry, "processStaticFieldRecord");
	return errCode;
}

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
int processBasicTypeValue(TagInfo * tagInfo, unsigned int entry, const char * source) {
	unsigned int typeOfEntry;
	unsigned int oneToFourByteValue;
	unsigned long long eightByteValue;
	int errCode;

	errCode = readByteToInt(tagInfo->stream, &typeOfEntry);
	tagInfo->dataLength = tagInfo->dataLength - 1;
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain typeOfEntry entry %d of %s\n", entry, source);
		return errCode;
	}

	fprintf(stdout, "typeOfEntry:%d, ", typeOfEntry);

	switch (typeOfEntry) {

	case BASIC_TYPE_BOOLEAN:
	case BASIC_TYPE_BYTE:
		errCode = readByteToInt(tagInfo->stream, &oneToFourByteValue);
		if (errCode != 0) {
			fprintf(stderr, "\nCould not obtain typeOfEntry %d entry %d of %s\n", typeOfEntry, entry, source);
			return errCode;
		}
		fprintf(stdout, "value:%d\n", oneToFourByteValue);
		tagInfo->dataLength = tagInfo->dataLength - 1;
		break;


	case BASIC_TYPE_CHAR:
	case BASIC_TYPE_SHORT:
		errCode = readTwoByteBigEndianStreamToInt(tagInfo->stream, &oneToFourByteValue);
		if (errCode != 0) {
			fprintf(stderr, "\nCould not obtain typeOfEntry %d entry %d of %s\n", typeOfEntry, entry, source);
			return errCode;
		}
		fprintf(stdout, "value:%d\n", oneToFourByteValue);
		tagInfo->dataLength = tagInfo->dataLength - 2;
		break;

	case BASIC_TYPE_FLOAT:
	case BASIC_TYPE_INT:
		errCode = readBigEndianStreamToInt(tagInfo->stream, &oneToFourByteValue);
		if (errCode != 0) {
			fprintf(stderr, "\nCould not obtain typeOfEntry %d entry %d of %s\n", typeOfEntry, entry, source);
			return errCode;
		}
		if (typeOfEntry == BASIC_TYPE_INT) {
			fprintf(stdout, "value:%d\n", oneToFourByteValue);
		}
		else {
			fprintf(stdout, "value:%f\n", (float)oneToFourByteValue);
		}
		tagInfo->dataLength = tagInfo->dataLength - 4;
		break;
	case BASIC_TYPE_DOUBLE:
	case BASIC_TYPE_LONG:
		errCode = readBigWordSmallWordBigEndianStreamToLong(tagInfo->stream, &eightByteValue);
		if (errCode != 0) {
			fprintf(stderr, "\nCould not obtain typeOfEntry %d entry %d of %s\n", typeOfEntry, entry, source);
			return errCode;
		}
		if (typeOfEntry == BASIC_TYPE_LONG) {
			fprintf(stdout, "value:%lld\n", eightByteValue);
		}
		else {
			fprintf(stdout, "value:%Le\n", (long double)eightByteValue);
		}
		tagInfo->dataLength = tagInfo->dataLength - 8;
		break;
	case BASIC_TYPE_OBJECT:
		errCode = getId(tagInfo->stream, tagInfo->idSize, &eightByteValue);
		if (errCode != 0) {
			fprintf(stderr, "\nCould not obtain BASIC_TYPE_OBJECT entry %d of %s\n", entry, source);
			return errCode;
		}
		fprintf(stdout, "value:%lld\n", eightByteValue);
		tagInfo->dataLength = tagInfo->dataLength - 1 * tagInfo->idSize;
		break;
	default:
		fprintf(stderr, "\ntypeOfEntry %d not recognized entry %d of %s\n", typeOfEntry, entry, source);
		return -1;
		break;
	}

	return 0;
}


/*
ID field name string ID
u1 type of field : (See Basic Type)
*/
int processInstanceFieldRecord(TagInfo * tagInfo, unsigned int entry) {
	unsigned long long instanceFieldNameId;
	unsigned int typeOfEntry;

	int errCode = getId(tagInfo->stream, tagInfo->idSize, &instanceFieldNameId);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain instanceFieldNameId entry %d of processInstanceFieldRecord\n", entry);
		return errCode;
	}

	errCode = readByteToInt(tagInfo->stream, &typeOfEntry);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain typeOfEntry entry %d of processInstanceFieldRecord\n", entry);
		return errCode;
	}

	fprintf(stdout, "\tinstanceFieldNameId:%lld, typeOfEntry:%d\n", instanceFieldNameId, typeOfEntry);
	tagInfo->dataLength = 
		tagInfo->dataLength
		- 1 * tagInfo->idSize
		- 1 * 1;

	return 0;
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
	unsigned long long objId;
	unsigned int stackTraceSerialNumber;
	unsigned long long classobjectId;
	unsigned int numOfBytes;

	int errCode = getId(tagInfo->stream, tagInfo->idSize, &objId);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain objId of HEAP_INSTANCE_DUMP\n");
		return errCode;
	}
	errCode = readBigEndianStreamToInt(tagInfo->stream, &stackTraceSerialNumber);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain stackTraceSerialNumber of HEAP_INSTANCE_DUMP\n");
		return errCode;
	}
	errCode = getId(tagInfo->stream, tagInfo->idSize, &classobjectId);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain classobjectId of HEAP_INSTANCE_DUMP\n");
		return errCode;
	}
	errCode = readBigEndianStreamToInt(tagInfo->stream, &numOfBytes);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain numOfBytes of HEAP_INSTANCE_DUMP\n");
		return errCode;
	}

	fprintf(stdout, "HEAP_INSTANCE_DUMP objId:%lld, stackTraceSerialNumber:%d, classobjectId:%lld, numOfBytes:%d\n",
		objId, stackTraceSerialNumber, classobjectId, numOfBytes
	);
	tagInfo->dataLength = tagInfo->dataLength
		- 2 * tagInfo->idSize
		- 2 * 4;

	errCode = iterateThroughStream(tagInfo->stream, numOfBytes);
	// TODO build up the class model and use it to output the instance values
	if (errCode != 0) {
		return errCode;
	}
	tagInfo->dataLength = tagInfo->dataLength - numOfBytes;

	return 0;
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
	unsigned long long arrayObjId;
	unsigned int stackTraceSerialNumber;
	unsigned int numOfElements;
	unsigned long long arrayClassObjId;

	int errCode = getId(tagInfo->stream, tagInfo->idSize, &arrayObjId);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain arrayObjId of HEAP_OBJECT_ARRAY_DUMP\n");
		return errCode;
	}
	errCode = readBigEndianStreamToInt(tagInfo->stream, &stackTraceSerialNumber);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain stackTraceSerialNumber of HEAP_PRIMITIVE_ARRAY_DUMP\n");
		return errCode;
	}
	errCode = readBigEndianStreamToInt(tagInfo->stream, &numOfElements);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain numOfElements of HEAP_PRIMITIVE_ARRAY_DUMP\n");
		return errCode;
	}
	errCode = getId(tagInfo->stream, tagInfo->idSize, &arrayClassObjId);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain arrayClassObjId of HEAP_OBJECT_ARRAY_DUMP\n");
		return errCode;
	}

	fprintf(stdout, "HEAP_OBJECT_ARRAY_DUMP: arrayObjId:%lld, stackTraceSerialNumber:%d, numOfElements:%d, arrayClassObjId:%lld\n",
		arrayObjId, stackTraceSerialNumber, numOfElements, arrayClassObjId
	);
	tagInfo->dataLength =
		tagInfo->dataLength
		- 2 * tagInfo->idSize
		- 2 * 4;

	for (unsigned int i = 0; i < numOfElements; i++) {
		errCode = processHeapPrimitiveArrayRecord(tagInfo, BASIC_TYPE_OBJECT, i, "HEAP_OBJECT_ARRAY_DUMP");
		if (errCode != 0) {
			return errCode;
		}
	}

	return 0;
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
	unsigned long long arrayObjId;
	unsigned int stackTraceSerialNumber;
	unsigned int numOfElements;
	unsigned int typeOfEntry;

	int errCode = getId(tagInfo->stream, tagInfo->idSize, &arrayObjId);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain arrayObjId of HEAP_PRIMITIVE_ARRAY_DUMP\n");
		return errCode;
	}
	errCode = readBigEndianStreamToInt(tagInfo->stream, &stackTraceSerialNumber);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain stackTraceSerialNumber of HEAP_PRIMITIVE_ARRAY_DUMP\n");
		return errCode;
	}
	errCode = readBigEndianStreamToInt(tagInfo->stream, &numOfElements);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain numOfElements of HEAP_PRIMITIVE_ARRAY_DUMP\n");
		return errCode;
	}
	errCode = readByteToInt(tagInfo->stream, &typeOfEntry);
	if (errCode != 0) {
		fprintf(stderr, "Could not obtain typeOfEntry of HEAP_PRIMITIVE_ARRAY_DUMP\n");
		return errCode;
	}

	fprintf(stdout, "HEAP_PRIMITIVE_ARRAY_DUMP: arrayObjId:%lld, stackTraceSerialNumber:%d, numOfElements:%d, typeOfEntry%d\n",
		arrayObjId, stackTraceSerialNumber, numOfElements, typeOfEntry
	);

	tagInfo->dataLength =
		tagInfo->dataLength
		- 1 * tagInfo->idSize
		- 2 * 4
		- 1 * 1;

	for (unsigned int i = 0; i < numOfElements; i++) {
		errCode = processHeapPrimitiveArrayRecord(tagInfo, typeOfEntry, i, "HEAP_PRIMITIVE_ARRAY_DUMP");
		if (errCode != 0) {
			return errCode;
		}
	}

	return 0;
}

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
int processHeapPrimitiveArrayRecord(TagInfo * tagInfo, unsigned int typeOfEntry, unsigned int entry, const char * source) {
	unsigned int oneToFourByteValue;
	unsigned long long eightByteValue;
	int errCode;

	switch (typeOfEntry) {

	case BASIC_TYPE_BOOLEAN:
	case BASIC_TYPE_BYTE:
		errCode = readByteToInt(tagInfo->stream, &oneToFourByteValue);
		if (errCode != 0) {
			fprintf(stderr, "\nCould not obtain typeOfEntry %d entry %d of %s\n", typeOfEntry, entry, source);
			return errCode;
		}
		fprintf(stdout, "\tvalue:%d\n", oneToFourByteValue);
		tagInfo->dataLength = tagInfo->dataLength - 1;
		break;


	case BASIC_TYPE_CHAR:
	case BASIC_TYPE_SHORT:
		errCode = readTwoByteBigEndianStreamToInt(tagInfo->stream, &oneToFourByteValue);
		if (errCode != 0) {
			fprintf(stderr, "\nCould not obtain typeOfEntry %d entry %d of %s\n", typeOfEntry, entry, source);
			return errCode;
		}
		fprintf(stdout, "\tvalue:%d\n", oneToFourByteValue);
		tagInfo->dataLength = tagInfo->dataLength - 2;
		break;

	case BASIC_TYPE_FLOAT:
	case BASIC_TYPE_INT:
		errCode = readBigEndianStreamToInt(tagInfo->stream, &oneToFourByteValue);
		if (errCode != 0) {
			fprintf(stderr, "\nCould not obtain typeOfEntry %d entry %d of %s\n", typeOfEntry, entry, source);
			return errCode;
		}
		if (typeOfEntry == BASIC_TYPE_INT) {
			fprintf(stdout, "\tvalue:%d\n", oneToFourByteValue);
		}
		else {
			fprintf(stdout, "\tvalue:%f\n", (float)oneToFourByteValue);
		}
		tagInfo->dataLength = tagInfo->dataLength - 4;
		break;
	case BASIC_TYPE_DOUBLE:
	case BASIC_TYPE_LONG:
		errCode = readBigWordSmallWordBigEndianStreamToLong(tagInfo->stream, &eightByteValue);
		if (errCode != 0) {
			fprintf(stderr, "\nCould not obtain typeOfEntry %d entry %d of %s\n", typeOfEntry, entry, source);
			return errCode;
		}
		if (typeOfEntry == BASIC_TYPE_LONG) {
			fprintf(stdout, "\tvalue:%lld\n", eightByteValue);
		}
		else {
			fprintf(stdout, "\tvalue:%Le\n", (long double)eightByteValue);
		}
		tagInfo->dataLength = tagInfo->dataLength - 8;
		break;
	case BASIC_TYPE_OBJECT:
		errCode = getId(tagInfo->stream, tagInfo->idSize, &eightByteValue);
		if (errCode != 0) {
			fprintf(stderr, "\nCould not obtain BASIC_TYPE_OBJECT entry %d of %s\n", entry, source);
			return errCode;
		}
		fprintf(stdout, "\tvalue:%lld\n", eightByteValue);
		tagInfo->dataLength = tagInfo->dataLength - 1 * tagInfo->idSize;
		break;
	default:
		fprintf(stderr, "\ntypeOfEntry %d not recognized entry %d of %s\n", typeOfEntry, entry, source);
		return -1;
		break;
	}

	return 0;
}

