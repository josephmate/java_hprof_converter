#ifndef IOSTREAM_READER_HPP
#define IOSTREAM_READER_HPP

#include "StreamReader.hpp"
#include <stdio.h>

class IOStreamReader : StreamReader {

public:
	IOStreamReader(FILE * file);

	int readByte();
	unsigned int readUByte();

private:
	FILE * file;

};

#endif