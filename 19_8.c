#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>
#include<semaphore.h>

char sharedVariable = 'o';

sem_t mutexA;
sem_t mutexB;
sem_t mutexC;

pthread_mutex_t mutexForTheSharedVariable;

/*thread function definition*/
void* threadA(void* args)
{
	while(1) /*run until queue is empty*/
	{
		sem_wait(&mutexA);
		if(sharedVariable == 'o')
		{
			//code for X*Y
			printf("Thread A : code for x*y\n");

			pthread_mutex_lock(&mutexForTheSharedVariable);
			sharedVariable = 'a';
			pthread_mutex_unlock(&mutexForTheSharedVariable);

			sem_post(&mutexB);
		}
		else if(sharedVariable == 'b')
		{
			//code for (x+y) * (x+y)
			printf("Thread A : code for (x+y) * (x+y)\n");
			
			pthread_mutex_lock(&mutexForTheSharedVariable);
			sharedVariable = 'o';
			pthread_mutex_unlock(&mutexForTheSharedVariable);

            sem_post(&mutexA);
		}
	}
}

void* threadB(void* args)
{
	while(1)
	{
		sem_wait(&mutexB);

		if(sharedVariable == 'a')
		{
			//code for(x+y)/2
			printf("Thread B : code for(x*y)/2\n");
			
			pthread_mutex_lock(&mutexForTheSharedVariable);
			sharedVariable = 'b';
			pthread_mutex_unlock(&mutexForTheSharedVariable);

			sem_post(&mutexC);
		}
		else if(sharedVariable == 'c')
		{
			//code for(x*y)/(x+y)
			printf("Thread B : code for(x*y)/(x+y)\n");

			pthread_mutex_lock(&mutexForTheSharedVariable);
			sharedVariable = 'b';
			pthread_mutex_unlock(&mutexForTheSharedVariable);
			
			sem_post(&mutexA);
		}
	}
}

void* threadC(void* args)
{
	while(1)
	{
		sem_wait(&mutexC);

		if(sharedVariable == 'b')
		{
			//code for(x+y)/2
			printf("Thread C : code for x+y\n");

			pthread_mutex_lock(&mutexForTheSharedVariable);
			sharedVariable = 'c';
			pthread_mutex_unlock(&mutexForTheSharedVariable);

			sem_post(&mutexB);
		}
	}
}


int main()
{
    /*creating thread id*/
    pthread_t idA;
    pthread_t idB;
    pthread_t idC;

	// mutexA = sem_open("/mutexA",O_CREAT, 0644, 1);
	// mutexB = sem_open("/mutexB",O_CREAT, 0644, 0);
	// mutexC = sem_open("/mutexC",O_CREAT, 0644, 0);

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
