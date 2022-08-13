#include <stdio.h>      //needed for basic I/O and fclose(), fopen()
#include <stdlib.h>     //needed for exit() function
#include <unistd.h>     //needed for close(), read(), write(), fstat() functions
#include <string.h>     //needed for memcmp() function
#include <sys/socket.h> //needed for socket(), listen() function
#include <arpa/inet.h>  //needed for htonl(), ntohl(), htons()
#include <fcntl.h>      //needed for open() function
#include <time.h>       //needed for clock() function
#include <sys/stat.h>   //needed for open(), fstat() functions
#include <sys/types.h>  //needed for open(), fstat() functions
                        //some  historical (BSD) implementations required <sys/types.h> header file for socket()


#define BUFFER_SIZE 1000    //specifies the Buffer size for buffer used for reading and writing data
#define PORT_NUMBER 9021    //specifies the port address the TCP connection


void parent(char *sampleFile);                      //for parent part of the program
void child();                                       //for child part of the program
struct sockaddr_in setServerAddress(int port);      //for setting the server address
void sendFile(int socketDescriptor, char *file);    //for sending "file" through socket "socketDescriptor"
void receiveFile(int socketDescriptor,char *file);  //for receiving a file through socket "socketDescriptor" and saving it in "file"
void fileCompare(char *file1, char *file2);         //for comparing "file1" and "file2"



int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("Enter name of the file to be transferred.\nExecution Command : ./19_2B <file_name>\n");
        exit(1);
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
        snprintf(errorString, sizeof errorString, "receiveFile() : recv() : Error receiving file size of %s through socket", file);
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
        };
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

void parent(char *sampleFile)
{
    //create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    //specify an address for the server socket
    struct sockaddr_in server_address = setServerAddress(PORT_NUMBER);

    //bind() assigns the address specified by  "server_address" to the socket referred to by "serverSocket". 
    if(bind(serverSocket,(struct sockaddr *)&server_address,sizeof(server_address)) == -1)
        perror("parent() : Error in binding server socket\n");

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


    printf("\nParent : Sending file to Child...[%s => childProcess(process2)]\n",sampleFile);
    sendFile(clientSocket, sampleFile);


    receiveFile(clientSocket, "retransmittedFile.txt");
    printf("Parent : File received...[received file(from process2) => retransmittedFile.txt]\n");

    //Timer stops
    t = clock() - t;
    printf("\nTimer stops.\n");
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // calculate the elapsed time
    printf("The double transfer of files took %f seconds to finish.\n", time_taken);


	close(serverSocket);

    fileCompare(sampleFile,"retransmittedFile.txt");
}


void child()
{   
    //create a socket
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	
    //specify an address for the server socket
	struct sockaddr_in serverAddress = setServerAddress(PORT_NUMBER);
	if(connect(clientSocket,(struct sockaddr*) &serverAddress,sizeof(serverAddress)) == -1)
		perror("Child : Error connecting to the Server");



    receiveFile(clientSocket, "receivedFile.txt");
    printf("Child  : File received...[received file(from process1) => receivedFile.txt]\n");
    
    printf("Child  : Sending file to parent...[receivedFile.txt => parentProcess(process1)]\n");
    sendFile(clientSocket, "receivedFile.txt");

    close(clientSocket);
}


struct sockaddr_in setServerAddress(int port)
{
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;
    return server_address;
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