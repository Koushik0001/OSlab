/*
***********************************************************************************************************************
   
                                           ***** OPERATING SYSTEMS LAB *****
*   DATE : 01/09/2022
*   TEAM NO : 19
*   TEAM MEMBERS :      KOUSHIK MAHANTA(L)     ( 002011001106 )
                        DISHARI SAHA           ( 002011001107 )
                        TUSAR MUNDHRA          ( 002011001122 )
                        DEBARGHA DATTA         ( 002011001082 )

***********************************************************************************************************************

***********************************************************************************************************************

    ASSIGNMENT DETAILS :    Assignment 5

    Problem : 

        The objective of this assignment is to avoid deadlock.
	For this purpose define two global variables (Total_1 and Total_2) and initialize both of them to 10000000. 
	You should also have two mutexes to protect these two global variables. 
	You need to create three threads also (Th1, Th2 and Th3).

	The function of thread Th1 is to generate a random quantity (not more than 10) and subtract that quantity
	from Total_2 and add that quantity to the Total_1.
        Likewise,The function of thread Th2 is to generate a random quantity (not more than 10) and subtract that
	quantity from Total_1 and add that quantity to the Total_2.
 
	While manipulating the Totals, the Th1 and Th2 should lock both the mutex and then unlock it after changing
	the Totals.

	The function of the thread Th3 is to keep displaying the individual Totals and Grand Total (sum of Total_1 and Total_2). 		Make sure the displayed values are consistent.

	Note that this program should run forever.

	The order of locking and unlocking the Mutex is very important, because that’s what avoids a Deadlock or creates one. 		Once you correctly identify the order for avoiding Deadlock, you should upload the program. 

	Also, include that information (as a comment in your source file) regarding the order for creating Deadlock.
	Make sure there are enough printf in your program so that it can be clearly understood that there is no Deadlock.
    
    Solution :

        1) First two threads are created using pthread_create() 
   	   One thread(thread2) for adding a random number to Total_1 and subracting the same from Total_2
           Another thread (thread3) prints the values of Total_1 and Tota_2
	
	2) Parent Thread does exactly the opposite of thread2

	3) Two mutex variables lock1 and lock2 are used to achieve mutual exclusion

	4) The order of locks and unlocks in the threads for Creating Deadlock is :
																																													
	thread1()
	{
		lock(lock1)
		lock(lock2)
		//manipulate shared variables
		unlock(lock1)
		unlock(lock2)
	}

	thread2()
	{
		lock(lock2);
		lock(lock1);
		//manipulate shared variables
		unlock(lock2);
		unlock(lock1);
	}

***********************************************************************************************************************

***********************************************************************************************************************
    INPUT DESCRIPTION : 

        No input required.


    OUTPUT DESCRIPTION : 

        1) The thread3 prints the values of Total_1, Total_2 and the value of (Total_1 + Total_2)

***********************************************************************************************************************

***********************************************************************************************************************
    EXECUTION :

        Compilation Command     :  gcc 19_5.c -o 19_5 -pthread
        Execution Sequence      :  ./19_5

***********************************************************************************************************************

***********************************************************************************************************************
        
    SAMPLE INPUT  : ./19_5

    SAMPLE OUTPUT :
	Creating Thread
	Thread3 : Total_1 = 9942343	Thread3 : Total_2 = 10057657	Thread3 : Grand_Total = 20000000
	Thread3 : Total_1 = 10124749	Thread3 : Total_2 = 9875251	Thread3 : Grand_Total = 20000000
	Thread3 : Total_1 = 10140517	Thread3 : Total_2 = 9859483	Thread3 : Grand_Total = 20000000
	Thread3 : Total_1 = 10039344	Thread3 : Total_2 = 9960656	Thread3 : Grand_Total = 20000000
	Thread3 : Total_1 = 9901652	Thread3 : Total_2 = 10098348	Thread3 : Grand_Total = 20000000
	Thread3 : Total_1 = 10232192	Thread3 : Total_2 = 9767808	Thread3 : Grand_Total = 20000000
	Thread3 : Total_1 = 10296679	Thread3 : Total_2 = 9703321	Thread3 : Grand_Total = 20000000
	Thread3 : Total_1 = 10321236	Thread3 : Total_2 = 9678764	Thread3 : Grand_Total = 20000000
	Thread3 : Total_1 = 10517602	Thread3 : Total_2 = 9482398	Thread3 : Grand_Total = 20000000
	Thread3 : Total_1 = 10718443	Thread3 : Total_2 = 9281557	Thread3 : Grand_Total = 20000000
	Thread3 : Total_1 = 10519362	Thread3 : Total_2 = 9480638	Thread3 : Grand_Total = 20000000
	Thread3 : Total_1 = 10448733	Thread3 : Total_2 = 9551267	Thread3 : Grand_Total = 20000000
	Thread3 : Total_1 = 10363349	Thread3 : Total_2 = 9636651	Thread3 : Grand_Total = 20000000
	Thread3 : Total_1 = 10404608	Thread3 : Total_2 = 9595392	Thread3 : Grand_Total = 20000000
	^C


***********************************************************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 	//Header file for sleep()
#include <pthread.h>
#include<semaphore.h>


int Total_1 = 10000000;
int Total_2 = 10000000;

pthread_mutex_t lock1;//mutex for variable Total_1
pthread_mutex_t lock2;//mutex for variable Total_2

#define LOWER 0
#define UPPER 10

//This function generates a random integer in the range [LOWER, UPPER]
unsigned long getRandomInteger()
{
  	unsigned long lower = LOWER;
  	unsigned long upper = UPPER;
	unsigned long num = (rand() %(upper - lower + 1)) + lower;
	return num;
}

//this function is run in the thread3
void *thread3(void *vargp)
{
	while(1)
	{	
		//this thread sleeps for one second after each iteration
		sleep(1);
		pthread_mutex_lock(&lock1);
		pthread_mutex_lock(&lock2);

		printf("Thread3 : Total_1 = %d\t", Total_1);
		printf("Thread3 : Total_2 = %d\t", Total_2);
		printf("Thread3 : Grand_Total = %d\n", Total_1 + Total_2);

		pthread_mutex_unlock(&lock2);
		pthread_mutex_unlock(&lock1);
	}
}

//this function is run in the thread2
void* thread2(void *vargp)
{
	int randomNumber;
	while(1)
	{	
		//getting random integer
		randomNumber = getRandomInteger();
		
		//acquiring locks
		pthread_mutex_lock(&lock1);
		pthread_mutex_lock(&lock2);
		
		//manipulating shared variables
		Total_2 += randomNumber;
		Total_1 -= randomNumber;
		
		//releasing locks
		pthread_mutex_unlock(&lock2);
		pthread_mutex_unlock(&lock1);
	}
}

//this function is run in the main or parent thread
void* thread1()
{
	int randomNumber;
	while(1)
	{
		//getting random integer
		randomNumber = getRandomInteger();
		
		//acquiring locks
		pthread_mutex_lock(&lock1);
		pthread_mutex_lock(&lock2);
		
		//manipulating shared variables
		Total_1 += randomNumber;
		Total_2 -= randomNumber;
		
		//releasing locks
		pthread_mutex_unlock(&lock2);
		pthread_mutex_unlock(&lock1);
	}
}


int main()
{
	int i;
	pthread_t thread_id2;
	pthread_t thread_id3;

	printf("Creating Thread\n");
	
	//setting the seed for the rand() function
	srand(time(NULL));

	//creating thread3
	if(pthread_create(&thread_id3, NULL, thread3, NULL) != 0)
		perror("Error : Creating a thread3\n");
	//creating thread2
	if(pthread_create(&thread_id2, NULL, thread2, NULL) != 0)
		perror("Error : Creating a thread2\n");

	//executing thread1() function in the parent thread
	thread1();

	printf("After Thread\n");
	return 0;
}
