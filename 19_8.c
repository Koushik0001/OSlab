#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include<semaphore.h>
#include<string.h>

// structure to create a node with fields x, y and a pointer to the next node
struct node 
{
	int x;
	int y;
	struct node* next;
};
struct node* front = NULL; //points to front node of the queue or NULL (if the queue is empty)
struct node* rear = NULL;  //points to the end node of the queue or NULL (if the queue is empty)
void enqueue(int valueX, int valueY);	//inerts a node in the queue which have x and y fields holding the values valueX and valueY accordingly
void dequeue(int* valueX, int* valueY); //stores the values of x and y fields of the front node in addresses valueX and valueY without removing the node from the queue
void display();	//displays the contents of the queue
int isEmpty();	//returns 1 if the queue is empty otherwise returns 0
void delete(); 	//deletes the front node from the queue


sem_t semaphoreA;	//for controlling the execution of thread A
sem_t semaphoreB;	//for controlling the execution of thread B
sem_t semaphoreC;	//for controlling the execution of thread C
char previousThread = 'o';	//a variable shared among the threads and holds the name of the last executed thread
pthread_mutex_t mutexForThepreviousThread; //this mutex facilitates mutual exclusion while accessing variable 'previosThread'
void* threadA(void* args);	/*function for thread A*/
void* threadB(void* args);	/*function for thread B*/
void* threadC(void* args);	/*function for thread C*/

void printColumnIntger(int number);
void printColumnFloat(float number);
void printPair(int x, int y); 

void processCommandLineInputs(int argc, char **argv);

int main(int argc, char **argv)
{
	if((argc-1) % 2 != 0)
	{
		printf("\nIncorrect input. Values must be inserted in paires.\nEven number of arguments required.\n");
		printf("Execution Command : ./19_8 x y x y x y x y ....\n\n");
		exit(1);
	}
	processCommandLineInputs(argc, argv);
	int choice, valueX, valueY;
	while (choice != 3) 
	{
		printf("\n1.Insert a Pair\n2.Display Queue\n3.Process Queue\n");
		printf("Enter your choice : ");
		scanf("%d", &choice);
		switch (choice) 
		{
		case 1:
			printf("Enter x <space> y: ");
			scanf("%d %d", &valueX, &valueY);
			enqueue(valueX, valueY);
			break;
		case 2:
			display();
			break;
		case 3:
			break;
        default:
			printf("\nWrong Choice\n");
		}
	}
	printf("\n");
    if(isEmpty())
	{
		printf("Queue is empty. Nothing to process...\n\n");
		exit(0);
	}
	//creating thread id variables to hold thread ids
    pthread_t idA;
    pthread_t idB;
    pthread_t idC;
    
    //initialising semaphore variables
	sem_init(&semaphoreA, 0, 1);
	sem_init(&semaphoreB, 0, 0);
	sem_init(&semaphoreC, 0, 0);

	
	if(pthread_mutex_init(&mutexForThepreviousThread, NULL) == -1)
		printf("mutex variable could not be initialized\n");
    int ret;
   	
	printf("Pairs\t\t|\tA\t|\tB\t|\tC\t|\tB\t|\tA\t|\n");
	for(int i=0; i<97; i++)printf("-");

    /*creating thread*/
    ret = pthread_create(&idA,NULL,&threadA,NULL);
    if(ret==-1)
    {
        printf("Thread A not created.\n");
       	exit(1); 
    }
    
    ret = pthread_create(&idB,NULL,&threadB,NULL);
    if(ret== -1)
    {
        printf("Thread B not created.\n");
        exit(2); 
    }
    
    ret = pthread_create(&idC,NULL,&threadC,NULL);
    if(ret==-1)
    {
        printf("Thread C not created.\n");
        exit(3);
    }
    
	pthread_join(idA,NULL);
	pthread_join(idB, NULL);
    pthread_join(idC, NULL);

    return 0;
}

void* threadA(void* args)
{
	while (!isEmpty()) /*run until queue is empty*/
	{
        int x,y, result;
		sem_wait(&semaphoreA);
		
		if (previousThread == 'o')
		{
			dequeue(&x, &y);
			printPair(x, y);

			result = x*y;
			printColumnIntger(result);

			pthread_mutex_lock(&mutexForThepreviousThread);
			previousThread = 'a';
			pthread_mutex_unlock(&mutexForThepreviousThread);

			sem_post(&semaphoreB);
		}
		else if (previousThread == 'b')
		{
            dequeue(&x, &y);
			result = ((x+y) * (x+y));
            printColumnIntger(result);

            delete();
			pthread_mutex_lock(&mutexForThepreviousThread);
			previousThread = 'o';
			pthread_mutex_unlock(&mutexForThepreviousThread);

			sem_post(&semaphoreA);
		}
	}
	printf("\n\n");
    sem_post(&semaphoreB);
    sem_post(&semaphoreC);
}

void* threadB(void* args)
{
	while (1)
	{
		sem_wait(&semaphoreB);
        int x, y;
		float result;
		if (previousThread == 'a')
		{
            dequeue(&x, &y);
			result = (x*y/2);
			printColumnFloat(result);

			pthread_mutex_lock(&mutexForThepreviousThread);
			previousThread = 'b';
			pthread_mutex_unlock(&mutexForThepreviousThread);

			sem_post(&semaphoreC);
		}
		else if (previousThread == 'c')
		{
            dequeue(&x, &y);
			result = (x*y/((float)x+(float)y));
			printColumnFloat(result);

			pthread_mutex_lock(&mutexForThepreviousThread);
			previousThread = 'b';
			pthread_mutex_unlock(&mutexForThepreviousThread);

			sem_post(&semaphoreA);
		}
        if(isEmpty())
            break;
	}
}

void* threadC(void* args)
{
	while (1)
	{
		sem_wait(&semaphoreC);
        int x, y, result;
		if (previousThread == 'b')
		{
			dequeue(&x, &y);
			result = x+y;
			printColumnIntger(result);
            
			pthread_mutex_lock(&mutexForThepreviousThread);
			previousThread = 'c';
			pthread_mutex_unlock(&mutexForThepreviousThread);

			sem_post(&semaphoreB);
		}
        if(isEmpty())
            break;
	}
}


void enqueue(int valueX, int valueY) {
	struct node* ptr;
	ptr = (struct node*)malloc(sizeof(struct node));
	ptr -> x = valueX;
	ptr -> y = valueY;
	ptr -> next = NULL;
	if ((front == NULL) && (rear == NULL)) {
		front = rear = ptr;
	}
	else {
		rear -> next = ptr;
		rear = ptr;
	}
}


void dequeue(int* valueX, int* valueY)
{
	if (front != NULL) 
	{
		*valueX = front -> x;
		*valueY = front -> y;
	}
}


void delete()
{
	if (front != NULL)
	{
		struct node* temp = front;
		front = front -> next;
		free(temp);
	}
}

// Display all elements of the queue
void display() {
	struct node* temp;
	if ((front == NULL) && (rear == NULL)) {
		printf("\nQueue is Empty\n");
	}
	else 
	{
		printf("The queue is : \n");
		printf("\tx\t|\ty\t|\n");
		for(int i=0; i<34; i++)printf("-");
		printf("\n");


		temp = front;
		while (temp) 
		{
			printColumnIntger(temp -> x);
			printColumnIntger(temp -> y);
			printf("\n");
			temp = temp -> next;
		}
	}
}

int isEmpty()
{
    if(front == NULL)
        return 1;
    else 
        return 0;
}


void processCommandLineInputs(int argc,char **argv)
{
	for(int i=1; i<argc; i+=2)
		enqueue(atoi(argv[i]),atoi(argv[i+1]));
    printf("\n\n# %d pairs inserted through Commandline\n", (argc-1)/2);
}

void printColumnIntger(int number)
{
	char buffer[15];
	sprintf(buffer, "%d", number);

	if(strlen(buffer) >= 15)
		printf("%d|",number);
	else if(strlen(buffer) >= 9)
		printf("%14d\t|",number);
	else if(strlen(buffer) >= 8)
		printf("%13d\t|",number);
	else if(strlen(buffer) >= 7)
		printf("%12d\t|",number);
	else if(strlen(buffer) >= 6)
		printf("%11d\t|",number);
	else if(strlen(buffer) >= 5)
		printf("%10d\t|",number);
	else 
		printf("\t%d\t|",number);
}
void printColumnFloat(float number)
{
	char buffer[15];
	sprintf(buffer, "%.3f", number);

	if(strlen(buffer) >= 15)
		printf("%.3f|",number);
	else if(strlen(buffer) >= 9)
		printf("%14.3f\t|",number);
	else if(strlen(buffer) >= 8)
		printf("%13.3f\t|",number);
	else if(strlen(buffer) >= 7)
		printf("%12.3f\t|",number);
	else if(strlen(buffer) >= 6)
		printf("%11.3f\t|",number);
	else if(strlen(buffer) >= 5)
		printf("%10.3f\t|",number);
	else 
		printf("\t%.3f\t|",number);
}
void printPair(int x, int y)
{
	char bufferX[15], bufferY[15];
	sprintf(bufferX, "%d", x);
	sprintf(bufferY, "%d", y);
	
	if(strlen(bufferX)+strlen(bufferY) >= 12)
        printf("\n(%d, %d)|", x, y);
	else if(strlen(bufferX)+strlen(bufferY) >= 5 )
		printf("\n(%d, %d)\t|", x, y);
	else
		printf("\n(%d, %d)\t\t|", x, y);
}