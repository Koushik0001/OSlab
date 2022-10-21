#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>
#include<semaphore.h>

void processCommandLineInputs(int argc, char **argv);
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




int main(int argc, char **argv)
{
	if((argc-1) % 2 != 0)
	{
		printf("Incorrect input. Values must be inserted in paires.\n");
		printf("Execution Command : ./19_8 x y x y x y x y ....\n");
		exit(1);
	}
	processCommandLineInputs(argc, argv);
	int choice, valueX, valueY;
	while (choice != 3 && choice != 4) 
	{
		printf("\n1.Enqueue\n2.Display Queue\n3.Nothing more to insert\n4.Done\n");
		printf("#Enter your choice : ");
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
		case 4:
			break;
        default:
			printf("\nWrong Choice\n");
		}
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
        int x,y;
		sem_wait(&semaphoreA);
		if (previousThread == 'o')
		{
			dequeue(&x, &y);
            printf("\nPair(%d, %d)\n", x, y);
            printf("=====================\n");
			printf("Thread A : %d * %d = %d\n",x, y, x*y);

			pthread_mutex_lock(&mutexForThepreviousThread);
			previousThread = 'a';
			pthread_mutex_unlock(&mutexForThepreviousThread);

			sem_post(&semaphoreB);
		}
		else if (previousThread == 'b')
		{
            dequeue(&x, &y);
			printf("Thread A : (%d + %d) * (%d + %d) = %d\n",x,y,x,y,((x+y) * (x+y)));
            
            delete();
			pthread_mutex_lock(&mutexForThepreviousThread);
			previousThread = 'o';
			pthread_mutex_unlock(&mutexForThepreviousThread);

			sem_post(&semaphoreA);
		}
	}
    sem_post(&semaphoreB);
    sem_post(&semaphoreC);
}

void* threadB(void* args)
{
	while (1)
	{
		sem_wait(&semaphoreB);
        int x, y;
		if (previousThread == 'a')
		{
            dequeue(&x, &y);
			printf("Thread B : (%d * %d)/2 = %d\n",x, y, (x*y/2));

			pthread_mutex_lock(&mutexForThepreviousThread);
			previousThread = 'b';
			pthread_mutex_unlock(&mutexForThepreviousThread);

			sem_post(&semaphoreC);
		}
		else if (previousThread == 'c')
		{
            dequeue(&x, &y);
			printf("Thread B : (%d * %d)/(%d + %d) = %f\n", x,y,x,y,(float)(x*y/((float)x+(float)y)));

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
        int x, y;
		if (previousThread == 'b')
		{
			dequeue(&x, &y);
			printf("Thread C : (%d + %d) = %d\n",x,y,(x+y));
            
			pthread_mutex_lock(&mutexForThepreviousThread);
			previousThread = 'c';
			pthread_mutex_unlock(&mutexForThepreviousThread);

			sem_post(&semaphoreB);
		}
        if(isEmpty())
            break;
	}
}

// Enqueue() operation on a queue
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

// Dequeue() operation on a queue
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
	else {
		printf("The queue is \n");
		temp = front;
		while (temp) {
			printf("%d  %d\n", temp -> x, temp -> y);
			temp = temp -> next;
		}
		printf("NULL\n\n");
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
    printf("# %d pairs inserted through Commandline\n", (argc-1)/2);
}
