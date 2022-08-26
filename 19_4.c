/*
***********************************************************************************************************************
   
                                           ***** OPERATING SYSTEMS LAB *****
*   DATE : 26/08/2022
*   TEAM NO : 19
*   TEAM MEMBERS :      KOUSHIK MAHANTA(L)     ( 002011001106 )
                        DISHARI SAHA           ( 002011001107 )
                        TUSAR MUNDHRA          ( 002011001122 )
                        DEBARGHA DATTA         ( 002011001082 )

***********************************************************************************************************************

***********************************************************************************************************************

    ASSIGNMENT DETAILS :    Assignment 4

    Problem : 

        Objective of this programming assignment is to use mmap() call and observe page-fault using the ‘sar’ command.

	A big file (about 8GB) should be created using the ‘fallocate’ command. 
	This big file should be written with a single integer value (say X) at a specific offset (say F). 
	Both the integer value and the offset should be generated using a random function.
	Please do remember this random function should generate a quantity anywhere between 0 and 8G.
	The above big file should also be mapped in the virtual address space using mmap() call.
	Once it is mapped, the data should be read from the same specific offset (F).
	Now, if the data read is X`; then verify that X and X` are the same.
	In case of verification failure, an error message is to be printed.
	Note that, the offset value F can be anywhere between 0 and 8G.
	This sequence of writing and reading data to/from a specific offset and also verification should be put in a
	while loop to go forever.
	In another terminal execute the command ‘sar –B 1 1000’ to observe for the page fault.
	This command should be started before the above program is put under execution. 
	So, one can observe that the page faults are increasing, once the above program starts executing.
	The output of the program and the ‘sar’ command should be pasted as a comment at the beginning of the program file.
    
    Solution :

        1) The user provided file is opened in read and write mode
	2) The information about the file, including the size is collected in the stat structure using fstat() function
	3) The complete file is mapped in the virtual address space using the mmap() function call
	4) Then the getRandomInteger() funtion is used to generate random integrs in the range [LOWER, UPPER]
	   Two integers are generated one for the value to be written and one for the offset where the value to be
	   written, srand(time(NULL)) is used to set the seed for the rand() funcion 
	5) Then the writeInteger() and readInteger() functions are used to write and read the value in/from the offset
	6) Then the wrtten and read values are compared to see if they are equal and a message is printed accordingly
	7) These actions of generating, writting, and reading values are done inside an infinite loop
	8) "sar -B 1 1000", command is run in another terminal before executing the c program
	9) "Ctrl+c" is used to terminate the two proesses

***********************************************************************************************************************

***********************************************************************************************************************
    INPUT DESCRIPTION : 

        This program takes the file name of the file to be transferred, as commandline input.
        ./19_4 <file_name>


    OUTPUT DESCRIPTION : 

        1) This function prints the value written in the offset of the buffer and also the value read from the same
	   offset
	2) This also prints if the value written and read are the same or not

***********************************************************************************************************************

***********************************************************************************************************************
    EXECUTION :

        Compilation Command     :  gcc 19_4.c -o 19_4
        Execution Sequence      :  

            [TEAM:19@assignment4]$ fallocate -l 8G file19.txt
            [TEAM:19@assignment4]$ ./19_4 file19.txt 

***********************************************************************************************************************

***********************************************************************************************************************
        
    SAMPLE INPUT  : ./19_4 file19.txt

    SAMPLE OUTPUT :

		value written = 1923999672
		value read    = 1923999672
		The value written and read are same :)
		value written = 1308161019
		value read    = 1308161019
		The value written and read are same :)
		value written = 1329841289
		value read    = 1329841289
		The value written and read are same :)
		value written = 835802510 
		value read    = 835802510 
		The value written and read are same :)
		value written = 1535696955
		value read    = 1535696955
		The value written and read are same :)
		value written = 1276262723
		value read    = 1276262723
		The value written and read are same :)
		value written = 1286857846
		value read    = 1286857846
		The value written and read are same :)
		value written = 959968656 
		value read    = 959968656 
		The value written and read are same :)
		value written = 1089304845
		value read    = 1089304845
		The value written and read are same :)
		^C



        [TEAM:19@assignment4]$ sar -B 1 1000
       	Linux 5.15.0-46-generic (koushik-Inspiron-5567) 	26/08/22 	_x86_64_	(4 CPU)

	07:45:06 PM IST  pgpgin/s pgpgout/s   fault/s  majflt/s  pgfree/s pgscank/s pgscand/s pgsteal/s    %vmeff
	07:45:07 PM IST      0.00    100.00     78.00      0.00    280.00      0.00      0.00      0.00      0.00
	07:45:08 PM IST      0.00      0.00     17.00      0.00    150.00      0.00      0.00      0.00      0.00
	07:45:09 PM IST      0.00      0.00      4.00      0.00    304.00      0.00      0.00      0.00      0.00
	07:45:10 PM IST      0.00      0.00    128.00      0.00     65.00      0.00      0.00      0.00      0.00
	07:45:11 PM IST      0.00      0.00     32.00      0.00   2740.00      0.00      0.00      0.00      0.00
	07:45:12 PM IST      0.00      0.00     45.00      0.00    270.00      0.00      0.00      0.00      0.00
	07:45:13 PM IST      0.00    156.00      0.00      0.00     15.00      0.00      0.00      0.00      0.00
	07:45:14 PM IST      0.00      0.00      7.00      0.00   2385.00      0.00      0.00      0.00      0.00
	07:45:15 PM IST      0.00      0.00     57.00      0.00    696.00      0.00      0.00      0.00      0.00
	07:45:16 PM IST      0.00     16.00  30752.00  15166.00   1235.00      0.00      0.00      0.00      0.00
	07:45:17 PM IST      0.00     32.00  47691.00  23692.00    664.00      0.00      0.00      0.00      0.00
	07:45:18 PM IST      0.00     32.00  53822.00  26738.00   3130.00      0.00      0.00      0.00      0.00
	07:45:19 PM IST      0.00     32.00  52466.00  26027.00    616.00      0.00      0.00      0.00      0.00
	07:45:20 PM IST      0.00     32.00  49497.00  24569.00    357.00      0.00      0.00      0.00      0.00
	07:45:21 PM IST      0.00  53500.00  43511.00  21291.00    388.00      0.00      0.00      0.00      0.00
	07:45:22 PM IST      0.00     32.00  46318.00  22192.00    421.00      0.00      0.00      0.00      0.00
	07:45:23 PM IST      0.00     40.00  43137.00  20729.00    471.00      0.00      0.00      0.00      0.00
	07:45:24 PM IST      0.00     32.00  40822.00  19596.00    515.00      0.00      0.00      0.00      0.00
	07:45:25 PM IST      0.00     32.00  39986.00  19234.00    446.00      0.00      0.00      0.00      0.00
	07:45:26 PM IST      0.00  43096.00  34786.00  16611.00    646.00      0.00      0.00      0.00      0.00
	07:45:27 PM IST      0.00 184156.00  26053.00  11194.00    692.00      0.00      0.00      0.00      0.00
	07:45:28 PM IST      0.00 196868.00  21739.00   8298.00   1636.00      0.00      0.00      0.00      0.00
	07:45:29 PM IST      0.00 245872.00     56.00      0.00    503.00      0.00      0.00      0.00      0.00
	07:45:30 PM IST      0.00 240316.00     14.00      0.00    432.00      0.00      0.00      0.00      0.00
	07:45:31 PM IST      0.00 107884.00     16.00      0.00    396.00      0.00      0.00      0.00      0.00
	07:45:32 PM IST      0.00      0.00      0.00      0.00     31.00      0.00      0.00      0.00      0.00

	^CAverage:         0.00  41239.54  20424.38   9820.65    749.38      0.00      0.00      0.00      0.00


        
***********************************************************************************************************************
*/
#include <stdio.h>	//for basic I/O operations
#include <stdlib.h>	//for rand(), srand() function
#include <time.h>	//for time() function
#include <sys/mman.h>	//for mmap() function
#include <sys/stat.h>	//for fstat(), open() function
#include <unistd.h>	//for fstat(), close() function
#include <fcntl.h>	//for open() function
#include <sys/types.h>	//for fstat(), open() function


#define LOWER 0			//LOWER value for the getRandomInteger() function
#define UPPER 8589934592	//UPPER value for the getRandomInteger() function

unsigned long getRandomInteger();				// Generates Integer in the range [LOWER, UPPER]
int writeInteger(unsigned long offset, unsigned long value);	// Writes value in the offset specified
int readInteger(unsigned long offset);				// Reads value from the offset specfied

char *mapBuffer;	//this holds the address of the buffer created by the mmap() function call

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("Enter name of the large file.\nExecution Command : ./19_4 <file_name>\n");
        exit(1);
    }

    //opening the user specified file in read and write mode
    int fileDescriptor = open(argv[1], O_RDWR, S_IRUSR | S_IWUSR);
    
    //this structure holds the information about the user specified file
    struct stat sb;
    //getting the information of the file in the sb variable
    if(fstat(fileDescriptor, &sb) == -1)
    {
    	perror("Error getting file size");
    	exit(1);
    }

    //mapping the file in the vartiual address space
    if((mapBuffer = (char*)mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fileDescriptor, 0)) == MAP_FAILED)
    {
    	perror("Error mapping file");
    	exit(2);
    }
	
    //setting the seed value for the rand() function
    srand(time(NULL));
    unsigned long valueWritten, valueRead, offset;

    while(1)
    {
	//get a random integer in the valueWritten variable from the getRandomInteger() function
    	valueWritten = getRandomInteger();
	//get a random integer in the offset variable from the getRandomInteger() function
    	offset = getRandomInteger();
	
	//write the value in valueWritten variable in the offset position of the mapBuffer 
    	writeInteger(offset, valueWritten);
	//read a value in valueRead variable from the offset position of the mapBuffer 
    	valueRead = readInteger(offset);
	
	//check if the valueWritten and the valueRead are equal or not
    	if(valueWritten == valueRead)
    		printf("The value written and read are same :)\n");
    	else
    		printf("The value written and read are not same :(\n");
    }
    
    return 0;
}

int writeInteger(unsigned long offset, unsigned long value)
{
	printf("value written = %-10lu\n", value);
	sprintf(mapBuffer+offset,"%lu", value);
}

int readInteger(unsigned long offset)
{
	unsigned long valueRead;
	sscanf( mapBuffer+offset, "%lu", &valueRead);
	printf("value read    = %-10lu\n", valueRead);
	return valueRead;
}

unsigned long getRandomInteger()
{
  	unsigned long lower = LOWER;
  	unsigned long upper = UPPER;
	unsigned long num = (rand() %(upper - lower + 1)) + lower;
	return num;
}
