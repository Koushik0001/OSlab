#include <stdio.h>      //needed for basic I/O and fclose(), fopen()
#include <stdlib.h>     //needed for exit() function
#include <unistd.h>     //needed for close(), read(), write() functions
#include <string.h>     //needed for memcmp() function
#include <sys/socket.h> //needed for socket(), listen() function
#include <arpa/inet.h>  //needed for htonl(), ntohl(), htons()
#include <fcntl.h>      //needed for open() function
#include <time.h>       //contains declaration of clock() function
#include <sys/stat.h>   //needed for open(), fstat() functions
#include <sys/types.h>  //neede for open() functions
                        //some  historical (BSD) implementations required this header file for socket()


#define BUFFER_SIZE 1000
#define PORT_NUMBER 9021

void parent(char *sampleFile);  //for parent part of the program
void child();                   //for child part of the program

struct sockaddr_in setServerAddress(int port);  //for setting the server address

void sendFile(int socketDescriptor, char *file);    //for sending file using socket socketDescriptor
void receiveFile(int socketDescriptor,char *file);

void fileCompare(char *file1, char *file2);



int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("Enter name of the file to be transferred.\nExecution Command : ./19_2B <file_name>\n");
        exit(1);
    }

    if (fork() == 0)
		child();
    else
        parent(argv[1]);
}


void sendFile(int socketDescriptor,char *file)
{
    char errorString[100];

    int fileDescriptor = open(file, O_RDONLY | O_CREAT, 0666); 
    if (fileDescriptor ==-1) 
    {
        snprintf(errorString, sizeof errorString, "Error in openning %s", file);
        perror(errorString);
    }

    struct stat s;
    fstat(fileDescriptor, &s); // i get the size
    long int size = htonl(s.st_size);
    send(socketDescriptor, (void *)&size,sizeof(long int),0 );
    printf("filesize sender %ld\n",s.st_size);
    
    char *buffer = (char*) calloc(BUFFER_SIZE, sizeof(char));
    long int totalBytesSent = 0;
    long int bytesSent = 0;
    int bytesRead = 0;
    while(totalBytesSent < s.st_size)
    {
        bytesRead = read(fileDescriptor, buffer,BUFFER_SIZE);
        bytesSent = write(socketDescriptor, buffer, bytesRead);
        totalBytesSent += bytesSent;
        //printf("totalBytesSent = %ld\n",totalBytesSent);
    }
    free(buffer);
    if(close(fileDescriptor) == -1)
    {
        snprintf(errorString, sizeof errorString, "Error in closing %s", file);
        perror(errorString);
    }
}


void receiveFile(int socketDescriptor, char *file)
{
    char errorString[100];
    int fileDescriptor = open(file, O_WRONLY | O_CREAT, 0666); 
    if (fileDescriptor ==-1) 
    {
        snprintf(errorString, sizeof errorString, "Error in openning %s", file);
        perror(errorString);
    }

    long int size;
    if(recv(socketDescriptor, (void *)&size, sizeof(size),0)<0)
        perror("problem receiving\n");
    long int trueSize = ntohl(size);
    printf("filesize receiver %ld\n",trueSize);

    char *buffer = (char*) calloc(BUFFER_SIZE, sizeof(char));
    long int totalBytesReceived = 0;
    long int bytesReceived = 0;
    while(totalBytesReceived < trueSize)
    {
        bytesReceived = read(socketDescriptor, buffer,BUFFER_SIZE);
        totalBytesReceived += bytesReceived;
        //printf("totalBytesReceived = %ld\n",totalBytesReceived);
        write(fileDescriptor, buffer, bytesReceived);
    }
    
    free(buffer);
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
        perror("Parent : Error in binding server socket\n");

    //listen to incoming connection
    if(listen(serverSocket,1) == -1)
        perror("Parent : Error listening on the port");

    //accept incoming connection
    int clientSocket = accept(serverSocket,NULL, NULL);
    if(clientSocket == -1)
        perror("Parent : Error accepting incoming connection");

    clock_t t;

    //timer strats here
    t = clock();
    printf("Timer starts.\n");


    printf("Parent : Sending File (%s => process2(child))\n",sampleFile);
    sendFile(clientSocket, sampleFile);


    receiveFile(clientSocket, "retransmittedFile.txt");
    printf("Parent : File Received\n");


    //Timer stops
    t = clock() - t;
    printf("Timer stops.\n");
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
    printf("Child : File Received\n");
    
    printf("\nChild : Sending File\n");
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

// Function to compare the contents of two files
void fileCompare(char *file1, char *file2)
{
    char errorString[100];

    //opening user provided file in "read mode" for comparison
    FILE *fp1 = fopen(file1,"r");
    if(fp1 == NULL)
    {
        snprintf(errorString, sizeof errorString, "Error in openning %s", file1);
        perror(errorString);
    }

    //opening retransmittedFile.txt in "read mode" for comparison
    FILE* fp2 = fopen(file2,"r");
    if(fp2 == NULL)
    {
        snprintf(errorString, sizeof errorString, "Error in openning %s", file2);
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
        snprintf(errorString, sizeof errorString, "Error in closing %s", file1);
        perror(errorString);
    }
    if(fclose(fp2) == EOF)
    {
        snprintf(errorString, sizeof errorString, "Error in closing %s", file2);
        perror(errorString);
    }
}