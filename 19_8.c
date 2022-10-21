#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>
#include<semaphore.h>


// Structure to create a node with data and the next pointer
struct node 
{
	int x;
	int y;
	struct node* next;
};

struct node* front = NULL;
struct node* rear = NULL;

char sharedVariable = 'o';

sem_t mutexA;
sem_t mutexB;
sem_t mutexC;

pthread_mutex_t mutexForTheSharedVariable;

void* threadA(void* args);
void* threadB(void* args);
void* threadC(void* args);

void enqueue(int valueX, int valueY);
void dequeue(int* valueX, int* valueY);
void display();
int isEmpty();
void delete();
/*function definition for thread A*/



int main()
{
	int choice, valueX, valueY;
	while (choice != 3) 
	{
		printf("1.Enqueue\n2.Display Queue\n3.Done\n");
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
			break;
        default:
			printf("\nWrong Choice\n");
		}
	}
	
    /*creating thread id*/
    pthread_t idA;
    pthread_t idB;
    pthread_t idC;

	sem_init(&mutexA, 0, 1);
	sem_init(&mutexB, 0, 0);
	sem_init(&mutexC, 0, 0);

	int value;
	sem_getvalue(&mutexA, &value);
	printf("mutex a = %d\n", value);
	sem_getvalue(&mutexB, &value);
	printf("mutex b = %d\n", value);
	sem_getvalue(&mutexC, &value);
	printf("mutex c = %d\n", value);

	
	if(pthread_mutex_init(&mutexForTheSharedVariable, NULL) == -1)
		printf("mutex variable could not be initialized\n");
    int ret;
   	
   	printf("i am here\n");
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
		sem_wait(&mutexA);
		if (sharedVariable == 'o')
		{
			dequeue(&x, &y);
            printf("\nPair(%d, %d)\n", x, y);
            printf("=====================\n");
			printf("Thread A : %d * %d = %d\n",x, y, x*y);

			pthread_mutex_lock(&mutexForTheSharedVariable);
			sharedVariable = 'a';
			pthread_mutex_unlock(&mutexForTheSharedVariable);

			sem_post(&mutexB);
		}
		else if (sharedVariable == 'b')
		{
            dequeue(&x, &y);
			printf("Thread A : (%d + %d) * (%d + %d) = %d\n",x,y,x,y,((x+y) * (x+y)));
            
            delete();
			pthread_mutex_lock(&mutexForTheSharedVariable);
			sharedVariable = 'o';
			pthread_mutex_unlock(&mutexForTheSharedVariable);

			sem_post(&mutexA);
		}
	}
    sem_post(&mutexB);
    sem_post(&mutexC);
}

void* threadB(void* args)
{
	while (1)
	{
		sem_wait(&mutexB);
        int x, y;
		if (sharedVariable == 'a')
		{
            dequeue(&x, &y);
			printf("Thread B : (%d * %d)/2 = %d\n",x, y, (x*y/2));

			pthread_mutex_lock(&mutexForTheSharedVariable);
			sharedVariable = 'b';
			pthread_mutex_unlock(&mutexForTheSharedVariable);

			sem_post(&mutexC);
		}
		else if (sharedVariable == 'c')
		{
            dequeue(&x, &y);
			printf("Thread B : (%d * %d)/(%d + %d) = %f\n", x,y,x,y,(float)(x*y/((float)x+(float)y)));

			pthread_mutex_lock(&mutexForTheSharedVariable);
			sharedVariable = 'b';
			pthread_mutex_unlock(&mutexForTheSharedVariable);

			sem_post(&mutexA);
		}
        if(isEmpty())
            break;
	}
}

void* threadC(void* args)
{
	while (1)
	{
		sem_wait(&mutexC);
        int x, y;
		if (sharedVariable == 'b')
		{
			dequeue(&x, &y);
			printf("Thread C : (%d + %d) = %d\n",x,y,(x+y));
            
			pthread_mutex_lock(&mutexForTheSharedVariable);
			sharedVariable = 'c';
			pthread_mutex_unlock(&mutexForTheSharedVariable);

			sem_post(&mutexB);
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
	printf("Node is Inserted\n\n");
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
