/*
***********************************************************************************************************************
   
                                           ***** OPERATING SYSTEMS LAB *****
*   DATE : 13/08/2022
*   TEAM NO : 19
*   TEAM MEMBERS :      KOUSHIK MAHANTA(L)     ( 002011001106 )
                        DISHARI SAHA           ( 002011001107 )
                        TUSAR MUNDHRA          ( 002011001122 )
                        DEBARGHA DATTA         ( 002011001082 )

***********************************************************************************************************************

***********************************************************************************************************************

    ASSIGNMENT DETAILS :    Assignment 3

    Problem : 

        Create two processes. 
        Transfer 1GB file from process1 to process2 using a Socket. 
        Now, transfer that file from process2 to process1 using the same Socket.
        Now, compare the two files to make sure that the same file has returned back.

        Also, print the time required to do this double transfer. 
        Attach this output to the source file as a comment.
        
        Please note that, you can see the socket which your program creates. 
        There are also various bash shell commands available to see the sockets created by the program. 
        So, once your program creates the socket, make sure you use proper command to see the socket info.
        Paste that output as a comment in your source file.
    
    Solution :

        1) fork() is used to create two processes
        2) Inside the parent process parent() function is invoked and inside child process child() function
        3) Inside parent() a TCP socket is created which serves as the server socket and is assigned an address,
            listen() is used to listen for incoming connection in this server socket 
        4) Inside child() another socket is created that serves as the client socket, a TCP connection is established
            between the client socket and the server socket by connect() function in child and accept()
            function in parent
    
        5) Now sendFile() and receiveFile() functions are used to transfer files between the two processes
        6) A timer is set before the trnasfer of files starts and the timer is stopped after the transfer is 
            finished. This givs us the time required for the double transfer.
        7) After that, the file provided by user and file "retransmittedFile.txt" are compared and result of the
            comparison is printed(same or not).

***********************************************************************************************************************

***********************************************************************************************************************
    INPUT DESCRIPTION : 

        This program takes the file name of the file to be transferred, as commandline input.
        ./19_3 <file_name>


    OUTPUT DESCRIPTION : 

        1)  The program prints messages indicating the initialization and completion of different events.
        For example : Starting of timer, Strting and Ending of file transfer, Ending of timer etc.

        2)  Program also prints the total time required for transferring a file, first from parent 
        process(process1) to child process(process2) and again transferring the same file from the child
        to parent.

        3) If the sent and receied back file in the parent process matches then the program prints that the
        files are same otherwise it prints that they are not same.

        4) As a result of the file transfers the program also generates one TCP socket, and two other files
        namely "receivedFile.txt" and "retransmittedFile.txt" which are deleted befor exiting the corresponding
        processes

***********************************************************************************************************************

***********************************************************************************************************************
    EXECUTION :

        Compilation Command     :  gcc 19_3.c -o 19_3
        Execution Sequence      :  

            [TEAM:19@assignment2]$ fallocate -l 1G file19.txt
            [TEAM:19@assignment2]$ ./19_3 file19.txt 

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
#include <stdio.h>      //needed for basic I/O and fclose(), fopen() functions
#include <stdlib.h>     //needed for exit() unction
#include <unistd.h>     //needed for close(), read(), write(), fstat() functions
#include <string.h>     //needed for memcmp() function
#include <sys/socket.h> //needed for socket(), listen() function
#include <arpa/inet.h>  //needed for htonl(), ntohl(), htons() functions
#include <fcntl.h>      //needed for open() function
#include <time.h>       //needed for clock() function
#include <sys/stat.h>   //needed for open(), fstat() functions
#include <errno.h>      //needed for getting the value of errno
#include <signal.h>     //needed for kill() function
#include <sys/types.h>  //needed for open(), fstat() functions
                        //some  historical (BSD) implementations required <sys/types.h> header file for socket()


#define BUFFER_SIZE 1000    //specifies the Buffer size for buffer used for reading and writing data
#define PORT_NUMBER 9021        //specifies the port address the TCP connection


void parent(char *sampleFiles);                     //for parent part of the program
void child();                                       //for child part of the program
struct sockaddr_in setServerAddress(int port);      //for setting the server address
int sendFile(int socketDescriptor, char *file);    //for sending "file" through socket "socketDescriptor"
int receiveFile(int socketDescriptor,char *file);  //for receiving a file through socket "socketDescriptor" and saving it in "file"
int fileCompare(char *file1, char *file2);         //for comparing "file1" and "file2"
void deleteFile(char *file);                        //for deleting "file"
void terminationSequence(char* file, int socketDescriptor);    //for releasing resources before process termination due to Errors


int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("Enter name of the file to be transferred.\nExecution Command : ./19_2B <file_name>\n");
        return -1;
    }
    //return value of fork() == 0 means its the child process
    if (fork() == 0)
    {
        //calling the child() function for excuting in the child process 
		child();
    }

    //else it is the parent process
    else
    {
        //calling the parent function and passing it the name of the file to be transferred
        //this will execute inside parent process
        parent(argv[1]);
    }
}


void parent(char *sampleFile)
{
    //for holding the name of the received back file
    char retransmittedFile[] = "retransmittedFile.txt";

    //create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket == -1)
    {
        perror("Parent : Error in creating socket");
        exit(1);
    }

    //specify an address for the server socket
    struct sockaddr_in server_address = setServerAddress(PORT_NUMBER);

    //bind() assigns the address specified by  "server_address" to the socket referred to by "serverSocket". 
    if(bind(serverSocket,(struct sockaddr *)&server_address,sizeof(server_address)) == -1)
    {
        if(errno == EADDRINUSE)
            printf("Port alreay in Use. Please change the value of PORT_ADDRESS in the code file\n");
        else
            perror("parent() : Error in binding server socket");
        close(serverSocket);
        exit(2);
    }

    //listen to incoming connection
    if(listen(serverSocket,1) == -1)
    {
        perror("parent() : Error listening on the port");
        close(serverSocket);
        exit(3);
    }

    //accept incoming connection
    int clientSocket = accept(serverSocket,NULL, NULL);
    if(clientSocket == -1)
    {
        perror("parent() : Error accepting incoming connection");
        terminationSequence(sampleFile, serverSocket);
        exit(4);
    }

    clock_t t;
    //timer strats here
    t = clock();
    printf("Timer starts.\n");

    //send user provided file through clientSicket
    printf("\nParent : Sending file to Child...[%s => childProcess(process2)]\n",sampleFile);
    if(sendFile(clientSocket, sampleFile) == -1)
    {
        terminationSequence(sampleFile, serverSocket);
        exit(5);
    }

    //receive the file sent through "clientSocket"
    if(receiveFile(clientSocket, retransmittedFile) == -1)
    {
        terminationSequence(sampleFile, serverSocket);
        deleteFile(retransmittedFile);
        exit(6);
    }
    printf("Parent : File received back from child...[received file => retransmittedFile.txt]\n");

    //Timer stops
    t = clock() - t;
    printf("\nTimer stops.\n");
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // calculate the elapsed time
    printf("The double transfer of files took %f seconds to finish.\n", time_taken);


    //close serverSocket
    if(close(serverSocket) == -1)
        perror("Parent : Error closing serverSocket");

    //comparing the two files
    if(fileCompare(sampleFile,retransmittedFile) == -1)
    {
        deleteFile(sampleFile);
        deleteFile(retransmittedFile);
        exit(8);
    }

    //deleting retransmittedFile.txt
    deleteFile(retransmittedFile);
}


void child()
{   
    //for holding the name of the received file
    char receivedFile[] = "receivedFile.txt";

    //create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSocket == -1)
    {
        perror("Child : Error in creating socket");
        exit(9);
    }
    
    //specify an address for the server socket
    struct sockaddr_in serverAddress = setServerAddress(PORT_NUMBER);
    if(connect(clientSocket,(struct sockaddr*) &serverAddress,sizeof(serverAddress)) == -1)
    {
        perror("Child : Error connecting to the Server");
        close(clientSocket);
    }


    //receive file from clientSocket
    if(receiveFile(clientSocket, receivedFile) == -1)
    {
        terminationSequence(receivedFile,clientSocket);
        exit(10);
    }
    printf("Child  : File received from parent...[received file => receivedFile.txt]\n");
    
    //send receivedFile.txt through clientSocket
    printf("Child  : Sending file back to parent...[receivedFile.txt => parentProcess(process1)]\n");
    if(sendFile(clientSocket, receivedFile) == -1)
    {
        deleteFile(receivedFile);
        terminationSequence(receivedFile, clientSocket);
        exit(10);
    }

    //close clientSocket
    if(close(clientSocket) == -1)
        perror("Parent : Error closing clientSocket");

    //deleting receivedFile.txt
    deleteFile(receivedFile);
}


struct sockaddr_in setServerAddress(int port)
{
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;
    return server_address;
}


int sendFile(int socketDescriptor,char *file)
{
    //creating a character array to hold the error messaage
    char errorString[100];

    //opening "file" in "read only" mode
    int fileDescriptor = open(file, O_RDONLY | O_CREAT, 0666); 
    if (fileDescriptor ==-1) 
    {
        snprintf(errorString, sizeof errorString, "sendFile() : open() : Error in openning %s", file);
        perror(errorString);
        return -1;
    }

    //this variable holds the information about the file indicated by "fileDescriptor"
    struct stat s;
    //getting the information about the file
    if(fstat(fileDescriptor, &s) == -1)
    {
        snprintf(errorString, sizeof errorString, "sendFile() : fstat() : Error reading information of %s", file);
        perror(errorString);
        return -1;
    }
    long int size = htonl(s.st_size);

    //sending the size of the "file" through socket
    if(send(socketDescriptor, (void *)&size,sizeof(long int),0 ) == -1)
    {
        snprintf(errorString, sizeof errorString, "sendFile() : send() : Error Sending file size of %s through socket", file);
        perror(errorString);
        return -1;
    }
    
    //creating a buffer for temporarily reading in the contents of files
    char *buffer = (char*) calloc(BUFFER_SIZE, sizeof(char));
    long int totalBytesSent = 0;
    long int bytesSent = 0;
    int bytesRead = 0;

    //this loop continues to execute when total number of bytes sent through socket < size of the "file"
    while(totalBytesSent < s.st_size)
    {
        //reading the "BUFFER_SIZE" bytes of "file" into the buffer
        bytesRead = read(fileDescriptor, buffer,BUFFER_SIZE);
        if(bytesRead == -1)
        {
            snprintf(errorString, sizeof errorString, "sendFile() : read() : Error reading in file %s", file);
            perror(errorString);
            return -1;
        }

        //sending "bytesRead" bytes of buffer through "socketDescriptor"
        bytesSent = write(socketDescriptor, buffer, bytesRead);
        if(bytesSent == -1)
        {
            snprintf(errorString, sizeof errorString, "sendFile() : write() : Error sending %s through socket", file);
            perror(errorString);
            return -1;
        }
        totalBytesSent += bytesSent;
    }

    //freeing up the buffer
    free(buffer);
    //closing the "file"
    if(close(fileDescriptor) == -1)
    {
        snprintf(errorString, sizeof errorString, "sendFile() : close() : Error in closing %s", file);
        perror(errorString);
        return -1;
    }
    return 0;
}


int receiveFile(int socketDescriptor, char *file)
{
    //creating a character array to hold the error messaage
    char errorString[100];

    //opening "file" in "write only" mode
    int fileDescriptor = open(file, O_WRONLY | O_CREAT, 0666); 
    if (fileDescriptor ==-1) 
    {
        snprintf(errorString, sizeof errorString, "receiveFile() : open() : Error in openning %s", file);
        perror(errorString);
        return -1;
    }

    //this holds the size of the file to be received
    long int size;
    //receiving the size of the file that it will receive next
    if(read(socketDescriptor, (void *)&size, sizeof(size)) == -1)
    {
        snprintf(errorString, sizeof errorString, "receiveFile() : read() : Error receiving file size of %s through socket", file);
        perror(errorString);
        return -1;
    }
    long int trueSize = ntohl(size);

    //creating a buffer for temporarily reading in the contents of files    
    char *buffer = (char*) calloc(BUFFER_SIZE, sizeof(char));
    long int totalBytesReceived = 0;
    long int bytesReceived = 0;

    //this loop executes when total number of bytes received through socket < size of the file to be received
    while(totalBytesReceived < trueSize)
    {
        //receiviing "BUFFER_SIZE" bytes from "socketDescriptor" into buffer
        bytesReceived = read(socketDescriptor, buffer,BUFFER_SIZE);
        if(bytesReceived == -1)
        {
            snprintf(errorString, sizeof errorString, "receiveFile() : read() : Error receiving file %s through socket", file);
            perror(errorString);
            return -1;
        }
        totalBytesReceived += bytesReceived;

        //writing "bytesReceived" bytes of buffer into "fileDescriptor"
        if(write(fileDescriptor, buffer, bytesReceived) == -1)
        {
            snprintf(errorString, sizeof errorString, "receiveFile() : write() : Error receiving %s through socket", file);
            perror(errorString); 
            return -1; 
        }
    }
    
    //freeing up the buffer
    free(buffer);
    //closing the file
    if(close(fileDescriptor) == -1)
    {
        snprintf(errorString, sizeof errorString, "Error in closing %s", file);
        perror(errorString);
        return -1;
    }

    return 0;
}


int fileCompare(char *file1, char *file2)
{
    printf("\nParent : Comparing files : %s and %s...\n", file1, file2);
    char errorString[100];

    //opening user provided file in "read mode" for comparison
    FILE *fp1 = fopen(file1,"r");
    if(fp1 == NULL)
    {
        snprintf(errorString, sizeof errorString, "fileCompare() : Error in openning %s", file1);
        perror(errorString);
        return -1;
    }

    //opening retransmittedFile.txt in "read mode" for comparison
    FILE* fp2 = fopen(file2,"r");
    if(fp2 == NULL)
    {
        snprintf(errorString, sizeof errorString, "fileCompare() : Error in openning %s", file2);
        perror(errorString);
        return -1;
    }

    char tmp1[512], tmp2[512];
    int isSame = 1;//0 means not same
    size_t bytes = 0, readsz = sizeof tmp1;

    while (!feof(fp1) || !feof(fp2))
    {
        fread (tmp1, sizeof *tmp1, readsz, fp1);
        fread (tmp2, sizeof *tmp2, readsz, fp2);
        if(memcmp(tmp1, tmp2, readsz))
        {
            isSame = 0;//files are not same
            printf("The files are not same :(\n");
        }
    }
    if(isSame == 1)
        printf("The files are same :)\n");

    if(fclose(fp1) == EOF)
    {
        snprintf(errorString, sizeof errorString, "fileCompare() : Error in closing %s", file1);
        perror(errorString);
        return -1;
    }
    if(fclose(fp2) == EOF)
    {
        snprintf(errorString, sizeof errorString, "fileCompare() : Error in closing %s", file2);
        perror(errorString);
        return -1;
    }
    return 0;
}

void deleteFile(char *file)
{
    char errorString[100];

    if(remove(file) == -1)
   {
        snprintf(errorString, sizeof errorString, "deleteFile() : Error in deleting %s", file);
        perror(errorString);
   }
}

void terminationSequence(char* file, int socketDescriptor)
{
    deleteFile(file);
    if(close(socketDescriptor) == -1)
        perror("terminationSequence() : Error closing socket");
}