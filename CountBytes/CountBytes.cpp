// CatFileTest.cpp : Defines the entry point for the console application.
//

#include <stdio.h>  
#include <fcntl.h>  
#include <io.h> 


int main()
{
	char buff;
	int numOfBytesRead;
	int dataLen = 0;

    #ifdef _WIN32
	int result = _setmode(_fileno(stdin), _O_BINARY);
	if (result == -1)
		perror("Cannot set mode");
	else
		printf("'stdin' successfully changed to binary mode\n");
	#endif

	while (true) {
		numOfBytesRead = fread(&buff, 1, 1, stdin);
		if (numOfBytesRead != 1) {
			if (feof(stdin)) {
				fprintf(stdout, "end of file reached\n");
			}
			int errorCode = ferror(stdin);
			if (errorCode) {
				fprintf(stdout, "error reading file %d\n", errorCode);
			}
			perror("The following error occurred:");
			break;
		}
		dataLen++;
	}
	fprintf(stdout, "read %d bytes\n", dataLen);
    return 0;
}

