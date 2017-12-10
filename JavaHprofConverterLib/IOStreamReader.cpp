#include "IOStreamReader.hpp"


IOStreamReader::IOStreamReader(FILE * file)
{
	this->file = file;
}

int IOStreamReader::readByte()
{
	unsigned char buff[1];
	size_t chunksRead = fread(buff, 1, 1, this->file);
	if (chunksRead != 1) {
		fprintf(stderr, "was unable to read a chunk of 1 bytes from the stream\n");
		return -1;
	}
	return (int)buff[0];
}

unsigned int IOStreamReader::readUByte()
{
	unsigned char buff[1];
	size_t chunksRead = fread(buff, 1, 1, this->file);
	if (chunksRead != 1) {
		fprintf(stderr, "was unable to read a chunk of 1 bytes from the stream\n");
		return -1;
	}
	return (unsigned int)buff[0];
}