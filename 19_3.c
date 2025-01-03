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

        4) As a result of the file transfers the program also generates TCP socket, and two other files
        namely "receivedFile.txt" and "retransmittedFile.txt" which are deleted befor exiting the corresponding
        processes

***********************************************************************************************************************

***********************************************************************************************************************
    EXECUTION :

        Compilation Command     :  gcc 19_3.c -o 19_3
        Execution Sequence      :  

            [TEAM:19@assignment3]$ fallocate -l 1G file19.txt
            [TEAM:19@assignment3]$ ./19_3 file19.txt 

***********************************************************************************************************************

***********************************************************************************************************************
        
    SAMPLE INPUT  : ./19_3 file19.txt

    SAMPLE OUTPUT :

            Timer starts.

            Parent : Sending file to Child...[file19.txt => childProcess(process2)]
            Child  : File received from parent...[received file => receivedFile.txt]
            Child  : Sending file back to parent...[receivedFile.txt => parentProcess(process1)]
            Parent : File received back from child...[received file => retransmittedFile.txt]

            Timer stops.
            The double transfer of files took 13.694014 seconds to finish.

            Parent : Comparing files : file19.txt and retransmittedFile.txt...
            The files are same :)


        [TEAM:19@assignment2]$ ss -t -a
            State    Recv-Q    Send-Q      Local Address:Port               Peer Address:Port    Process   
            LISTEN   0         50                0.0.0.0:netbios-ssn             0.0.0.0:*                 
            LISTEN   0         4096        127.0.0.53%lo:domain                  0.0.0.0:*                 
            LISTEN   0         128             127.0.0.1:ipp                     0.0.0.0:*                 
            LISTEN   0         1                 0.0.0.0:9021                    0.0.0.0:*                 
            LISTEN   0         50                0.0.0.0:microsoft-ds            0.0.0.0:*                 
            ESTAB    2616732   0               127.0.0.1:9021                  127.0.0.1:33754             
            ESTAB    0         1899007         127.0.0.1:33754                 127.0.0.1:9021              
            LISTEN   0         50                   [::]:netbios-ssn                [::]:*                 
            LISTEN   0         128                 [::1]:ipp                        [::]:*                 
            LISTEN   0         50                   [::]:microsoft-ds               [::]:* 
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
#define PORT_NUMBER 9021    //specifies the port address for the TCP connection


void parent(char *sampleFiles, int childId);        //for parent part of the program
void child();                                       //for child part of the program
struct sockaddr_in setServerAddress(int port);      //for setting the server address
void sendFile(int socketDescriptor, char *file);    //for sending "file" through socket "socketDescriptor"
void receiveFile(int socketDescriptor,char *file);  //for receiving a file through socket "socketDescriptor" and saving it in "file"
void fileCompare(char *file1, char *file2);         //for comparing "file1" and "file2"
void deleteFile(char *file);                        //for deleting "file"


int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("Enter name of the file to be transferred.\nExecution Command : ./19_3 <file_name>\n");
        exit(1);
    }
    int pid = fork();
    //return value of fork() == 0 means its the child process
    if (pid == 0)
    {
        //calling the child() function for excuting in the child process 
		child();
    }

    //else it is the parent process
    else
    {
        //calling the parent function and passing it the name of the file to be transferred and child process id
        //this will execute inside parent process
        parent(argv[1], pid);
    }
}


void parent(char *sampleFile, int childId)
{
    //for holding the name of the received back file
    char retransmittedFile[] = "retransmittedFile.txt";

    //create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket == -1)
        perror("Parent : Error in creating socket");

    //specify an address for the server socket
    struct sockaddr_in server_address = setServerAddress(PORT_NUMBER);

    //bind() assigns the address specified by  "server_address" to the socket referred to by "serverSocket". 
    if(bind(serverSocket,(struct sockaddr *)&server_address,sizeof(server_address)) == -1)
    {
        if(errno == EADDRINUSE)
        {
            printf("Port alreay in Use. Please change the value of PORT_ADDRESS in the code file\n");
            int ret = kill(childId, SIGINT);
            if (ret == -1) 
            {
                perror("Error in Killing Child process");
                exit(EXIT_FAILURE);
            }
            exit(2);
        }
        else
            perror("parent() : Error in binding server socket");
    }

    //listen to incoming connection
    if(listen(serverSocket,1) == -1)
        perror("parent() : Error listening on the port");

    //accept incoming connection
    int clientSocket = accept(serverSocket,NULL, NULL);
    if(clientSocket == -1)
        perror("parent() : Error accepting incoming connection");

    clock_t t;
    //timer strats here
    t = clock();
    printf("Timer starts.\n");

    //send user provided file through clientSicket
    printf("\nParent : Sending file to Child...[%s => childProcess(process2)]\n",sampleFile);
    sendFile(clientSocket, sampleFile);

    //receive the file sent through "clientSocket"
    receiveFile(clientSocket, retransmittedFile);
    printf("Parent : File received back from child...[received file => retransmittedFile.txt]\n");

    //Timer stops
    t = clock() - t;
    printf("\nTimer stops.\n");
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // calculate the elapsed time
    printf("The double transfer of files took %f seconds to finish.\n", time_taken);


    //close serverSocket
    if(close(serverSocket) == -1)
        perror("Parent : Error closing serverSocket");

    printf("\nParent : Comparing files : %s and %s...\n", sampleFile, retransmittedFile);
    //comparing the two files
    fileCompare(sampleFile,retransmittedFile);

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
        perror("Child : Error in creating socket");
    
    //specify an address for the server socket
    struct sockaddr_in serverAddress = setServerAddress(PORT_NUMBER);
    if(connect(clientSocket,(struct sockaddr*) &serverAddress,sizeof(serverAddress)) == -1)
        perror("Child : Error connecting to the Server");


    //receive file from clientSocket
    receiveFile(clientSocket, receivedFile);
    printf("Child  : File received from parent...[received file => receivedFile.txt]\n");
    
    //send receivedFile.txt through clientSocket
    printf("Child  : Sending file back to parent...[receivedFile.txt => parentProcess(process1)]\n");
    sendFile(clientSocket, receivedFile);

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


void sendFile(int socketDescriptor,char *file)
{
    //creating a character array to hold the error messaage
    char errorString[100];

    //opening "file" in "read only" mode
    int fileDescriptor = open(file, O_RDONLY | O_CREAT, 0666); 
    if (fileDescriptor ==-1) 
    {
        snprintf(errorString, sizeof errorString, "sendFile() : open() : Error in openning %s", file);
        perror(errorString);
    }

    //this variable holds the information about the file indicated by "fileDescriptor"
    struct stat s;
    //getting the information about the file
    if(fstat(fileDescriptor, &s) == -1)
    {
        snprintf(errorString, sizeof errorString, "sendFile() : fstat() : Error reading information of %s", file);
        perror(errorString);
    }
    long int size = htonl(s.st_size);

    //sending the size of the "file" through socket
    if(send(socketDescriptor, (void *)&size,sizeof(long int),0 ) == -1)
    {
        snprintf(errorString, sizeof errorString, "sendFile() : send() : Error Sending file size of %s through socket", file);
        perror(errorString);
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
        }

        //sending "bytesRead" bytes of buffer through "socketDescriptor"
        bytesSent = write(socketDescriptor, buffer, bytesRead);
        if(bytesSent == -1)
        {
            snprintf(errorString, sizeof errorString, "sendFile() : write() : Error sending %s through socket", file);
            perror(errorString);
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
    }
}


void receiveFile(int socketDescriptor, char *file)
{
    //creating a character array to hold the error messaage
    char errorString[100];

    //opening "file" in "write only" mode
    int fileDescriptor = open(file, O_WRONLY | O_CREAT, 0666); 
    if (fileDescriptor ==-1) 
    {
        snprintf(errorString, sizeof errorString, "receiveFile() : open() : Error in openning %s", file);
        perror(errorString);
    }

    //this holds the size of the file to be received
    long int size;
    //receiving the size of the file that it will receive next
    if(read(socketDescriptor, (void *)&size, sizeof(size)) == -1)
    {
        snprintf(errorString, sizeof errorString, "receiveFile() : read() : Error receiving file size of %s through socket", file);
        perror(errorString);
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
        }
        totalBytesReceived += bytesReceived;

        //writing "bytesReceived" bytes of buffer into "fileDescriptor"
        if(write(fileDescriptor, buffer, bytesReceived) == -1)
        {
            snprintf(errorString, sizeof errorString, "receiveFile() : write() : Error receiving %s through socket", file);
            perror(errorString);
        }
    }
    
    //freeing up the buffer
    free(buffer);
    //closing the file
    if(close(fileDescriptor) == -1)
    {
        snprintf(errorString, sizeof errorString, "Error in closing %s", file);
        perror(errorString);
    }
}


void fileCompare(char *file1, char *file2)
{
    char errorString[100];

    //opening user provided file in "read mode" for comparison
    FILE *fp1 = fopen(file1,"r");
    if(fp1 == NULL)
    {
        snprintf(errorString, sizeof errorString, "fileCompare() : Error in openning %s", file1);
        perror(errorString);
    }

    //opening retransmittedFile.txt in "read mode" for comparison
    FILE* fp2 = fopen(file2,"r");
    if(fp2 == NULL)
    {
        snprintf(errorString, sizeof errorString, "fileCompare() : Error in openning %s", file2);
        perror(errorString);
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
    }
    if(fclose(fp2) == EOF)
    {
        snprintf(errorString, sizeof errorString, "fileCompare() : Error in closing %s", file2);
        perror(errorString);
    }
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