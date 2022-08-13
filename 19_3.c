#include <stdio.h>
#include <stdlib.h>     //needed for exit() function
#include <unistd.h>     //needed for close(), read(), write() functions
#include <sys/mman.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

#include <fcntl.h>      //needed for open() function
#include <time.h>       //contains declaration of clock() function
#include <sys/stat.h>   //needed for open(), mkfifo() functions
#include <sys/types.h>  //neede for open(), mkfifo() functions


#define BUFFER_SIZE 1000

void parent(char *sampleFile);
void child();

void sendFile(int socketDescriptor, int fileDescriptor);
void receiveFile(int socketDescriptor,int fileDescriptor);


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

    if (fork() == 0)
    {
		child();
    }
    else
    {
        parent(argv[1]);
    }
}


void sendFile(int socketDescriptor,int fileDescriptor)
{
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
    close(fileDescriptor);
}

void receiveFile(int clientSocket,int fileDescriptor)
{
    long int size;
    if(recv(clientSocket, (void *)&size, sizeof(size),0)<0)
        perror("problem receiving\n");
    long int trueSize = ntohl(size);
    printf("filesize receiver %ld\n",trueSize);

    char *buffer = (char*) calloc(BUFFER_SIZE, sizeof(char));
    long int totalBytesReceived = 0;
    long int bytesReceived = 0;
    while(totalBytesReceived < trueSize)
    {
        bytesReceived = read(clientSocket, buffer,BUFFER_SIZE);
        totalBytesReceived += bytesReceived;
        //printf("totalBytesReceived = %ld\n",totalBytesReceived);
        write(fileDescriptor, buffer, bytesReceived);
    }
    
    free(buffer);
    close(fileDescriptor);
}

void parent(char *sampleFile)
{
    //create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    //specify an address for the server socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket,(struct sockaddr *)&server_address,sizeof(server_address));
    listen(serverSocket,5);
    int clientSocket = accept(serverSocket,NULL, NULL);

    // Calculate the time taken by take_enter()
    clock_t t;
    //timer strats here
    t = clock();
    printf("Timer starts.\n");
    printf("Parent : Sending File\n");



//****************************************************************************
    //opening the sampleFile in "read only" mode
    int sampleFileDescriptor = open(sampleFile, O_RDONLY | O_CREAT, 0666); 
    if (sampleFileDescriptor ==-1) 
        // print error description in the event of failure
        perror("\nParent : Error in openning Sample file");
    sendFile(clientSocket, sampleFileDescriptor);
    printf("\n****Parent : File sent \n");
//****************************************************************************

//****************************************************************************
    int retransmittedFileDescriptor = open("retransmittedFile.txt", O_WRONLY | O_CREAT, 0666); 
    if (retransmittedFileDescriptor == -1) 
        // print error description in the event of failure 
        perror("\nChild : Error in openning receivedFile.txt");
    receiveFile(clientSocket, retransmittedFileDescriptor);
    close(retransmittedFileDescriptor);
    printf("Child : File Received\n");
//****************************************************************************

    //Timer stops
    t = clock() - t;
    printf("Timer stops.\n");
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // calculate the elapsed time
    printf("The double transfer of files took %f seconds to finish.\n", time_taken);
    
    //closing retransmittedFile.txt
    if(close(retransmittedFileDescriptor) == -1)
        perror("Parent : Error closing retransmittedFile.txt");
	close(serverSocket);
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
void child()
{   
    //create a socket
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	
    //specify an address for the server socket
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(9002);
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	int connection_status = connect(clientSocket,(struct sockaddr*) &serverAddress,sizeof(serverAddress));
	if(connection_status <0)// check for any error in making the connection
		printf("There is a problem in making connection\n");

//***********************************************************************************
    int receivedFileDescriptor = open("receivedFile.txt", O_WRONLY | O_CREAT, 0666); 
    if (receivedFileDescriptor == -1) 
        // print error description in the event of failure 
        perror("\nChild : Error in openning receivedFile.txt");
    receiveFile(clientSocket, receivedFileDescriptor);
    printf("Child : File Received\n");
    //if(close(receivedFileDescriptor)<0)
    //    printf("Error in closing received\n");
    //struct stat s;
    //fstat(receivedFileDescriptor, &s); // i get the size
    //printf("filesize inside child %ld\n",s.st_size);
//************************************************************************************

//***********************************************************************************
    //opening the sampleFile in "read only" mode
    int receivedFileDescriptor2 = open("receivedFile.txt", O_RDONLY); 
    if (receivedFileDescriptor2 ==-1) 
         //print error description in the event of failure
        perror("\nParent : Error in openning Sample file");
    sendFile(clientSocket, receivedFileDescriptor);
    close(receivedFileDescriptor2);
    printf("\nParent : File sent \n");
//************************************************************************************

    close(clientSocket);
}