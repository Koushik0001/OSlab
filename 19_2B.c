/*
***********************************************************************************************************************
   
                                           ***** OPERATING SYSTEMS LAB *****
*   DATE : 06/08/2022
*   TEAM NO : 19
*   TEAM MEMBERS :      KOUSHIK MAHANTA(L)     ( 002011001106 )
                        DISHARI SAHA           ( 002011001107 )
                        TUSAR MUNDHRA          ( 002011001122 )
                        DEBARGHA DATTA         ( 002011001082 )

***********************************************************************************************************************

***********************************************************************************************************************

    ASSIGNMENT DETAILS :    Assignment 2B

    Problem : 

        Create two processes. Transfer 1GB file from process1 to process2 using a FIFO. 
        Now, transfer the same file from process2 to process1 using another FIFO. 
        Now, compare the two files to make sure that the same file has returned back. 
        Also, print the time required to do this double transfer. 
        Attach this output to the source file as a comment.
        To create FIFO, you can either use shell command or system call.
        
        To create a large file you can use the relevant command.
        Use "ls –l" command to show the FIFO and the large file.
        Attach this output to the source file as a comment.
    
    Solution :

        1) Two FIFO pipes are created : "fifoFile1", "fifoFile2"
        2) fork() systemcall is used to generate a child process(process2) of the current process(process1)
        3) In the parent Process data is read into a buffer from a file provided by user and written into 
            "fifoFile1" in a while loop using read and write calls
        4) In the child process data is first read into buffer and then written into the file "receivedFile.txt"
        5) The child process then writes the data read from "receivedFile.txt" into fifoFile2
        6) Parent process then reads data from "fifoFile2" and writes it into "retransmittedFile.txt"
        7) A timer is set before the trnsfer of files starts and the timer is stopped after the transfer is 
            finished. This givs us the time required by the double transfer.
        8) After that the file provided by user and file "retransmittedFile.txt" are compared and result of the
            comparison is printed(same or not).

***********************************************************************************************************************

***********************************************************************************************************************
    INPUT DESCRIPTION : 

        This program takes the file name of the file to be transferred, as commandline input.
        ./19_2B <file_name>


    OUTPUT DESCRIPTION : 

        1)  The program prints messages indicating the initialization and completion of different events.
        For example : Starting of timer, Strting and Ending of file transfer, Ending of timer etc.

        2)  Program also prints the total time required for transferring a file, first from parent 
        process(process1) to child process(process2) and again transferring the same file from the child
        to parent.

        3) If the sent and receied back file in the parent process matches then the program prints that the
        files are same otherwise it prints that they are not same.

        4) As a result of the file transfers the program also generates two FIFO("fifoFile1" and "fifoFile2") 
        files, and two other files namely "receivedFile.txt" and "retransmittedFile.txt"

***********************************************************************************************************************

***********************************************************************************************************************
    EXECUTION :

        Compilation Command     :  gcc 19_2B.c -o 19_2B
        Execution Sequence      :  

            [TEAM:19@assignment2]$ fallocate -l 1G file19.txt
            [TEAM:19@assignment2]$ ./19_2B file19.txt 

***********************************************************************************************************************

***********************************************************************************************************************
        
    SAMPLE INPUT  : ./19_2B file19.txt

    SAMPLE OUTPUT :

            Timer starts.
            Parent : Sending file to Child...[file19.txt => childProcess(process2)]
            Child  : File received...[received file(from process1) => receivedFile.txt]
            Child  : Sending file to parent...[receivedFile.txt => parentProcess(process1)]
            Parent : File received...[received file(from process2) => retransmittedFile.txt]
            Timer stops.
            The double transfer of files took 21.894872 seconds to finish.
            The files are same :)


        [TEAM:19@assignment2]$ ls -l
            total 3145780
            -rwxr--r-- 1 koushik koushik       2636 Aug  2 19:33 19_2A.c
            -rwxrwxr-x 1 koushik koushik      16632 Aug  6 15:22 19_2B
            -rwxr--r-- 1 koushik koushik      15604 Aug  6 15:22 19_2B.c
            prw-rw-r-- 1 koushik koushik          0 Aug  6 15:23 fifoFile1
            prw-rw-r-- 1 koushik koushik          0 Aug  6 15:23 fifoFile2
            -rw-rw-r-- 1 koushik koushik 1073741824 Aug  6 15:14 file19.txt
            -rw-rw-r-- 1 koushik koushik 1073741824 Aug  6 15:23 receivedFile.txt
            -rw-rw-r-- 1 koushik koushik 1073741824 Aug  6 15:23 retransmittedFile.txt

***********************************************************************************************************************
*/
#include <stdio.h>      //needed for basic I/O and perror() function
#include <fcntl.h>      //needed for open() function
#include <time.h>       //contains declaration of clock() function
#include <sys/stat.h>   //needed for open(), mkfifo() functions
#include <sys/types.h>  //neede for open(), mkfifo() functions     
#include <unistd.h>     //needed for close(), read(), write() functions
#include <stdlib.h>     //needed for exit() function
#include <string.h>     //needed for memcmp() function

#define BUFFER_SIZE 1000

// Function to compare the contents of two files
int fileCompare(FILE *fp1, FILE *fp2)
{
    char tmp1[16], tmp2[16];

    size_t bytes = 0, readsz = sizeof tmp1;
    int count = 0;
    while (!feof(fp1) || !feof(fp2))
    {
        fread (tmp1, sizeof *tmp1, readsz, fp1);
        fread (tmp2, sizeof *tmp2, readsz, fp2);
        count += 16;
        if(memcmp(tmp1, tmp2, readsz))
            return 1;//files are not same
    }
    return 0;//files are same
}

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("Enter name of the file to be transferred.\nExecution Command : ./19_2B <file_name>\n");
        exit(1);
    }

    char *FIFOfile1 = "fifoFile1";
    char *FIFOfile2 = "fifoFile2";

    //creating fifoFile1 to transfer file from parent process(process1) to child process(process2)
    if(mkfifo(FIFOfile1, 0666) == -1)
        perror("Error creating fifoFile1");

    //creating fifoFile2 to transfer file from child process(process2) to parent process(process1)
    if(mkfifo(FIFOfile2, 0666) == -1)
        perror("Error creating fifoFile2"); 


    //child process because return value of fork() is zero
    //this peocess first receives a file from the parent and then sends the file back to the parent
    if (fork() == 0)
    {
        /** this portion of child process receives a file through fifoFile1 **/

        //this variable will hold the bytes read during read() funciton call
        int bytesRead;

        //creating a buffer for temporarily reading in the contents of files
        char *buffer =(char*) calloc(BUFFER_SIZE, sizeof(char));

        //creting a file to save the contents received through pipe in "write only" mode
        int receivedFileDescriptor = open("receivedFile.txt", O_WRONLY | O_CREAT, 0666); 
        if (receivedFileDescriptor == -1) 
            // print error description in the event of failure 
            perror("\nChild : Error in openning receivedFile.txt");                 

        //opening the fifoFile1 in "read only" mode
        int fifoDescriptor = open(FIFOfile1,O_RDONLY);
        if (fifoDescriptor ==-1) 
            // print error description in the event of failure 
            perror("\nChild : Error in openning fifoFile1"); 

        //reading the "BUFFER_SIZE" bytes of fifoFile into the buffer
        bytesRead = read(fifoDescriptor,(void*)buffer,BUFFER_SIZE);

        //bytesRead == 0, signifies end of file
        while(bytesRead != 0)
        {
            //writing the contents of the buffer to the newly created receivedFile.txt
            if(write(receivedFileDescriptor,buffer,bytesRead) == -1)
                perror("\nChild : Error writing receivedFile.txt");

            //reading the next 'BUFFER_SIZE' number of bytes into the buffer from fifoFile1
            bytesRead = read(fifoDescriptor,buffer,BUFFER_SIZE);
            if(bytesRead == -1)
                perror("\nChild : Error reading in from fifoFile1");
        }
        printf("Child  : File received...[received file(from process1) => receivedFile.txt]\n");

        //closing the fifoFile1
        if(close(fifoDescriptor) == -1)
            perror("Child : Error closing fifoFile1");
        //closing the receivedFile.txt
        if(close(receivedFileDescriptor) == -1)
            perror("Child : Error closing receivedFile.txt");


        /******************************************************************************************************/
        /** this portion child process sends back the received contents through fifoFile2**/

        //opening the receivedFile.txt in "read only" mode
        receivedFileDescriptor = open("receivedFile.txt", O_RDONLY); 
        if (receivedFileDescriptor ==-1) 
            //print error description in the event of failure  
            perror("\nChild : Error in openning receivedFile.txt");                 

        //opening the fifoFile2 in "write only" mode
         fifoDescriptor = open(FIFOfile2,O_WRONLY);
        if (fifoDescriptor ==-1) 
            //print error description in the event of failure  
            perror("\nChild : Error in openning fifoFile2"); 

        //reading the "BUFFER_SIZE" bytes of receivedFile.txt into the buffer 
        bytesRead = read(receivedFileDescriptor,(void*)buffer,BUFFER_SIZE);
        if(bytesRead == -1)
            perror("\nChild : Error reading in from receivedFile.txt");

        printf("Child  : Sending file to parent...[receivedFile.txt => parentProcess(process1)]\n");
        while(bytesRead != 0)
        {
            //writing 'bytesRead' number of bytes into fifoFile2
            if(write(fifoDescriptor,buffer,bytesRead)==-1)
                perror("\nChild : Error writing fifoFile2");

            //reading 'BUFFER_SIZE' bytes of receivedFile.txt into buffer
            bytesRead = read(receivedFileDescriptor,buffer,BUFFER_SIZE);
            if(bytesRead == -1)
                perror("\nChild : Error reading in from receivedFile.txt");
        }

        //closing receivedFile.txt
        if(close(receivedFileDescriptor) == -1)
            perror("Child : Error closing receivedFile.txt");
        //closing fifoFile2
        if(close(fifoDescriptor) == -1)
            perror("Child : Error closing fifoFile2");
        
    }


    //parent process enters here because return value of fork() is not zero
    //this parent process first sends a file to child and then receives the file back from child
    //it then compares the sent and received files
    else
    {
        /** this portion of parent process sends the sampleFile.txt through fifoFile1**/


        //this variable will hold the bytes read during read() funciton call
        int bytesRead;

        char *sampleFile = argv[1];
        //creating a buffer for temporarily reading in the contents of files
        char *buffer =(char*) calloc(BUFFER_SIZE, sizeof(char));

        //opening the sampleFile in "read only" mode
        int sampleFileDescriptor = open(sampleFile, O_RDONLY | O_CREAT, 0666); 
        if (sampleFileDescriptor ==-1) 
            // print error description in the event of failure
            perror("\nParent : Error in openning Sample file");                 

        //opening fifoFile1 in "write only" mode
        int fifoDescriptor = open(FIFOfile1,O_WRONLY);
        if (fifoDescriptor ==-1) 
            // print error description in the event of failure 
            perror("\nParent : Error in openning fifoFile1"); 
	
        // Calculate the time taken by take_enter()
        clock_t t;
        //timer strats here
        t = clock();
        printf("Timer starts.\n");

	   printf("Parent : Sending file to Child...[%s => childProcess(process2)]\n",sampleFile);
	
        //reading the "BUFFER_SIZE" bytes of sampleFile into the buffer
        bytesRead = read(sampleFileDescriptor,(void*)buffer,BUFFER_SIZE);
        //bytesRead == 0 indicates end of file
        while(bytesRead != 0)
        {
            //writing "bytesRead" number of bytes into fifoFile1
            if(write(fifoDescriptor,buffer,bytesRead)==-1)
                perror("\nParent : Error writing fifoFile1");

            //reading "BUFFER_SIZE" bytes of sampleFile into buffer
            bytesRead = read(sampleFileDescriptor,buffer,BUFFER_SIZE);
            if(bytesRead == -1)
                perror("\nParent : Error reading in from sampleFile.txt");
        }

        //closing samoleFile
        if(close(sampleFileDescriptor) == -1)
            perror("Parent : Error closing userProvidedFile");
        //closing fifoFile1
        if(close(fifoDescriptor) == -1)
            perror("Parent : Error closing fifoFile1");


        /***************************************************************************************************/
        /** this portion of parent process receives back the file through fifoFile2 **/

        //opening retransmittedFile.txt in "write only mode"
        int retransmittedFileDescriptor = open("retransmittedFile.txt", O_WRONLY | O_CREAT, 0666); 
        if (retransmittedFileDescriptor == -1)
            // print error description in the event of failure 
            perror("\nParent : Error in openning retransmittedFile");                 

        //opening fifoFile2 in "read only" mode
        fifoDescriptor = open(FIFOfile2,O_RDONLY);
        if (fifoDescriptor ==-1) 
            // print error description in the event of failure
            perror("\nParent : Error in openning fifoFile2 : "); 

        //reading the "BUFFER_SIZE" bytes of fifoFile into the buffer
        bytesRead = read(fifoDescriptor,(void*)buffer,BUFFER_SIZE);
        if(bytesRead == -1)
            perror("\nParent : Error reading in from fifoFile2");
        while(bytesRead != 0)
        {   
            //writing "bytesRead" number of bytes into retransmittedFile.txt
            if(write(retransmittedFileDescriptor,buffer,bytesRead) == -1)
                perror("\nParent : Error writing retransmittedFile.txt");

            //reading 'BUFFER_SIZE' bytes of fifoFile2 into buffer
            bytesRead = read(fifoDescriptor,buffer,BUFFER_SIZE);
            if(bytesRead == -1)
                perror("\nParent : Error reading in from fifoFile2");
        }
        printf("Parent : File received...[received file(from process2) => retransmittedFile.txt]\n");
	
        //Timer stops
        t = clock() - t;
        printf("Timer stops.\n");
        double time_taken = ((double)t)/CLOCKS_PER_SEC; // calculate the elapsed time
        printf("The double transfer of files took %f seconds to finish.\n", time_taken);

        //closing fifoFile2
        if(close(fifoDescriptor) == -1)
            perror("Parent : Error closing fifoFile2");
        //closing retransmittedFile.txt
        if(close(retransmittedFileDescriptor) == -1)
            perror("Parent : Error closing retransmittedFile.txt");

        //opening sample File in "read mode" for comparison
        FILE *sentFile = fopen(sampleFile,"r");
        if(sentFile == NULL)
		  perror("Error opening UserProvidedFile for Comparing");

        //opening retransmittedFile.txt in "read mode" for comparison
        FILE* recvdbackFile = fopen("retransmittedFile.txt","r");
        if(recvdbackFile == NULL)
		  perror("Error opening retransmittedFile.txt for comparison");

        if(fileCompare(sentFile,recvdbackFile) == 0)
            printf("The files are same :)\n");
        else
            printf("The files are not same :(\n");
    }
}    
