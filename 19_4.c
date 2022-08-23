#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define LOWER 0
#define UPPER 8589934592

unsigned long getRandomInteger();							// Generates Integers in the range [LOWER, UPPER]
int writeInteger(unsigned long offset, unsigned long value);	//
int readInteger(unsigned long offset);

char *mapBuffer;

int main(int argc, char **argv)
{
	if(argc < 2)
    {
        printf("Enter name of the large file.\nExecution Command : ./19_4 <file_name>\n");
        exit(1);
    }

    int fileDescriptor = open(argv[1], O_RDWR, S_IRUSR | S_IWUSR);
    struct stat sb;
    if(fstat(fileDescriptor, &sb) == -1)
    {
    	perror("Error getting file size");
    	exit(1);
    }

    if((mapBuffer = (char*)mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fileDescriptor, 0)) == MAP_FAILED)
    {
    	perror("Error mapping file");
    	exit(2);
    }

    srand(time(NULL));
    unsigned long valueWritten, valueRead, offset;

    while(1)
    {
    	valueWritten = getRandomInteger();
    	offset = getRandomInteger();

    	writeInteger(offset, valueWritten);
    	valueRead = readInteger(offset);

    	if(valueWritten == valueRead)
    		printf("The value written and read are same :)\n");
    	else
    		printf("The value written and read are not same :(\n");
    }
	return 0;
}

int writeInteger(unsigned long offset, unsigned long value)
{
	printf("written = %lu\n", value);
	sprintf(mapBuffer+offset,"%lu", value);
}

int readInteger(unsigned long offset)
{
	unsigned long valueRead;
	sscanf( mapBuffer+offset, "%lu", &valueRead);
	printf("read = %lu\n", valueRead);
	return valueRead;
}

unsigned long getRandomInteger()
{
  	unsigned long lower = LOWER;
  	unsigned long upper = UPPER;
	unsigned long num = (rand() %(upper - lower + 1)) + lower;
	return num;
}
