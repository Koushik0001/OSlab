#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>

int Total_1 = 1000;
int Total_2 = 1000;
pthread_mutex_t lock1;
pthread_mutex_t lock2;

#define LOWER 0
#define UPPER 50

unsigned long getRandomInteger()
{
  	unsigned long lower = LOWER;
  	unsigned long upper = UPPER;
	unsigned long num = (rand() %(upper - lower + 1)) + lower;
	return num;
}

void manipulate(int threadNum)
{
	srand(time(NULL));
	int randomNumber = getRandomInteger();
	if(threadNum == 1)
	{
		Total_1 -= randomNumber;
		Total_2 += randomNumber;
	}
	else
	{
		Total_2 -= randomNumber;
		Total_1 += randomNumber;
	}
}

// A normal C function that is executed as a thread
// when its name is specified in pthread_create()
void* thread2(void *vargp)
{
	int randomNumber;
	while(1)
	{
		randomNumber = getRandomInteger();

		printf("Thread2 : Waiting to acquire lock1\n");
		pthread_mutex_lock(&lock1);
		printf("Thread2 : lock1 acquired\n");
		printf("Thread2 : Waiting to acquire lock2\n");
		pthread_mutex_lock(&lock2);
		printf("Thread2 : lock2 acquired\n");
		
		manipulate(2);
		printf("Thread2 : Total_1 = %d\n", Total_1);
		printf("Thread2 : Total_2 = %d\n", Total_2);

		printf("Thread2 : Realising lock 2\n");
		pthread_mutex_unlock(&lock2);
		printf("Thread2 : Realising lock 1\n");
		pthread_mutex_unlock(&lock1);
	}
}
void* thread1()
{
	int randomNumber;
	while(1)
	{
		randomNumber = getRandomInteger();
		printf("Thread1 : Waiting to acquire lock2\n");
		pthread_mutex_lock(&lock2);
		printf("Thread2 : lock2 acquired\n");
		printf("Thread1 : Waiting to acquire lock1\n");
		pthread_mutex_lock(&lock1);
		printf("Thread2 : lock1 acquired\n");

	
		manipulate(1);
		printf("Thread1 : Total_2 = %d\n", Total_1);
		printf("Thread2 : Total_1= %d\n", Total_2);

		printf("Thread2 : Realising lock 2\n");
		pthread_mutex_unlock(&lock2);
		printf("Thread2 : Realising lock 1\n");
		pthread_mutex_unlock(&lock1);
	}
}

int main()
{
	int i;
	pthread_t thread_id;
	srand(time(NULL));

	printf("Before Thread\n");
	pthread_create(&thread_id, NULL, thread2, NULL);
	thread1();
	printf("After Thread\n");
	exit(0);
}
