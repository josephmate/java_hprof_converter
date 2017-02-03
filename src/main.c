// ConsoleApplication1.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#ifdef _WIN32
#include <fcntl.h>  
#endif

#define SIZE_OF_HEADER 31
#define TRUE 1

void print_header(char * header);
int print_tag(char tagType, FILE * f);
int from_little_endian_stream_to_int(char * data);
int from_big_endian_stream_to_int(char * data);

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

}

#define TAG_HEADER_SIZE 8
int print_tag(char tagType, FILE * f) {
	unsigned char buff[TAG_HEADER_SIZE];
	unsigned char * buffPointer = &buff;
	size_t numOfBytesRead;
	fprintf(stdout, "tag type: %d\n", (int)tagType);  fflush(stdout);

	numOfBytesRead = fread(buff, TAG_HEADER_SIZE, 1, f);
	if (numOfBytesRead != 1) {
		fprintf(stderr,
			"Was reading tag header. expected %d bytes, but was %d\n",
			TAG_HEADER_SIZE, numOfBytesRead
		);
	}
	fprintf(stdout, "char array: %d %d %d %d         %d %d %d %d\n",
		buff[0], buff[1], buff[2], buff[3],
		buff[4], buff[5], buff[6], buff[7]
	);
	fflush(stdout);

	int microsSince = from_big_endian_stream_to_int(buffPointer);
	fprintf(stdout, "micros since start: %d\n", microsSince); fflush(stdout);
	int dataLength = from_big_endian_stream_to_int(buffPointer +4);
	fprintf(stdout, "data length: %d\n", dataLength); fflush(stdout);

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
int from_little_endian_stream_to_int(unsigned char * data) {
	return  (data[0] << 0) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
}
int from_big_endian_stream_to_int(unsigned char * data) {
	return  (data[3] << 0) | (data[2] << 8) | (data[1] << 16) | (data[0] << 24);
}