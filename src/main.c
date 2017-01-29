// ConsoleApplication1.cpp : Defines the entry point for the console application.
//
#include <stdio.h>

#define SIZE_OF_HEADER 32
#define TRUE 1

void print_header(char * header);
int print_tag(char tagType, FILE * f);
int to_int(char * bytes);

int main()
{	
	size_t numOfBytesRead;
	FILE * f = stdin;
	char buff[256];

	// read the header
	numOfBytesRead = fread(buff, SIZE_OF_HEADER, 1, f);
	if (numOfBytesRead != 1) {
		fprintf(stderr, "Was expecting the hprof binary file to contain at least %d bytes, but we were only able to read 0\n",
			SIZE_OF_HEADER);
		return -1;
	}
	print_header(buff);

	// read the tag until there are no more
	while(TRUE) {
		numOfBytesRead = fread(&buff, 1, 1, f);
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
 * header is a 32 byte array containing the header data
 */
void print_header(char * header) {

}

#define TAG_HEADER_SIZE 8
int print_tag(char tagType, FILE * f) {
	char buff[256];
	char * charPointer = &buff;
	size_t numOfBytesRead;
	fprintf(stdout, "tag type: %d\n", (int)tagType);  fflush(stdout);

	numOfBytesRead = fread(buff, TAG_HEADER_SIZE, 1, f);
	if (numOfBytesRead != 1) {
		fprintf(stderr,
			"Was reading tag header. expected %d bytes, but failed but was 0\n",
			TAG_HEADER_SIZE
		);
	}
	int microsSince = (int)(*charPointer);
	fprintf(stdout, "micros since start: %d\n", microsSince); fflush(stdout);
	charPointer += 4;
	int dataLength = (int)(*charPointer);
	fprintf(stdout, "data length: %d\n", dataLength); fflush(stdout);

	for (int i = 0; i < dataLength; i++) {
		numOfBytesRead = fread(buff, 1, 1, f);
		if (numOfBytesRead != 1) {
			fprintf(stderr,
				"Was reading entry. expected %d bytes, but failed on %d\n",
				dataLength, i
			);
			return -1;
		}
	}
}


int to_int(char * bytes) {
	char result[2];
	result[0] = bytes[1];
	return 0;
}