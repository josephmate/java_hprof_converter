#ifndef STREAM_READER_HPP
#define STREAM_READER_HPP

class StreamReader {
public:
	virtual int readByte() = 0;
	virtual unsigned int readUByte() = 0;
	//virtual int readNibble() = 0;
	//virtual int readInt() = 0;
	//virtual long long readLong() = 0;
	//virtual char* readByteArray(int len) = 0;
	//virtual void skip(long long len) = 0;
	//virtual void close() = 0;
};

#endif