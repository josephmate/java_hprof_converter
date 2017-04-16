#include "StreamUtil.h"


int posnOfNull(unsigned char * str, int maxLen) {
	for (int i = 0; i < maxLen; i++) { //only checking 31-12 characters because we need the long and the size of id
		if (str[i] == '\0') {
			return i;
		}
	}
	return -1;
}

unsigned int fromTwoByteBigEndianStreamToInt(unsigned char * data) {
	return  (data[1] << 0) | (data[0] << 8);
}
// http://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c
unsigned int fromLittleEndianStreamToInt(unsigned char * data) {
	return  (data[0] << 0) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
}
unsigned int fromBigEndianStreamToInt(unsigned char * data) {
	return  (data[3] << 0) | (data[2] << 8) | (data[1] << 16) | (data[0] << 24);
}
unsigned long long fromBigWordSmallWordBigEndianWords(unsigned char * data) {
	return    ((unsigned long long)data[3] << 32) | ((unsigned long long)data[2] << 40) | ((unsigned long long)data[1] << 48) | ((unsigned long long)data[0] << 56)
	        | ((unsigned long long)data[7] <<  0) | ((unsigned long long)data[6] <<  8) | ((unsigned long long)data[5] << 16) | ((unsigned long long)data[4] << 24);
}


int readByteToInt(FILE * f, unsigned int * result) {
	unsigned char buff[1];
	size_t chunksRead = fread(buff, 1, 1, f);
	if (chunksRead != 1) {
		fprintf(stderr, "was unable to read a chunk of 1 bytes from the stream\n");
		return -1;
	}
	*result = (unsigned int)buff[0];
	return 0;
}

int readTwoByteBigEndianStreamToInt(FILE * f, unsigned int * result) {
	char buff[2];
	size_t chunksRead = fread(buff, 2, 1, f);
	if (chunksRead != 1) {
		fprintf(stderr, "was unable to read a chunk of 2 bytes from the stream\n");
		return -1;
	}
	*result = fromTwoByteBigEndianStreamToInt(buff);
	return 0;
}

int readBigEndianStreamToInt(FILE * f, unsigned int * result) {
	char buff[4];
	size_t chunksRead = fread(buff, 4, 1, f);
	if (chunksRead != 1) {
		fprintf(stderr, "was unable to read a chunk of 4 bytes from the stream\n");
		return -1;
	}
	*result = fromBigEndianStreamToInt(buff);
	return 0;
}

int readBigWordSmallWordBigEndianStreamToLong(FILE * f, unsigned long long * result) {
	char buff[8];
	size_t chunksRead = fread(buff, 8, 1, f);
	if (chunksRead != 1) {
		fprintf(stderr, "was unable to read a chunk of 4 bytes from the stream\n");
		return -1;
	}
	*result = fromBigWordSmallWordBigEndianWords(buff);
	return 0;
}


/**
* Iterates through dataLength bytes of the stream: f.
*/
int iterateThroughStream(FILE* f, long long dataLength) {
	unsigned char buff[1];
	unsigned char * buffPointer = buff;
	size_t numOfBytesRead;
	for (int i = 0; i < dataLength; i++) {
		numOfBytesRead = fread(buffPointer, 1, 1, f);
		if (numOfBytesRead != 1) {
			fprintf(stderr,
				"Was reading data of tag entry. expected %lld bytes, but failed on %d\n",
				dataLength, i
			);
			perror("The following error occurred:");
			return -1;
		}
	}
	return 0;
}

int getId(FILE * f, int idSize, unsigned long long * result) {
	char buff[8];
	if (idSize == 4) {
		fread(buff, 4, 1, f);
		*result = fromBigEndianStreamToInt(buff);
		return 0;
	}
	else if (idSize == 8) {
		fread(buff, 8, 1, f);
		*result = fromBigWordSmallWordBigEndianWords(buff);
		return 0;
	}
	fprintf(stderr, "idSize %d is not supported\n", idSize);
	return -1;
}