/*
***********************************************************************************************************************
   
                                           ***** OPERATING SYSTEMS LAB *****
*   DATE : 21/10/2022
*   TEAM NO : 19
*   TEAM MEMBERS :      KOUSHIK MAHANTA(L)     ( 002011001106 )
                        DISHARI SAHA           ( 002011001107 )
                        TUSAR MUNDHRA          ( 002011001122 )
                        DEBARGHA DATTA         ( 002011001082 )

***********************************************************************************************************************

***********************************************************************************************************************

    ASSIGNMENT DETAILS :    Assignment 8

    Problem : 
		    Consider a process with three threads A, B, and C. 
			The default thread of the process receives two inputs (X, Y) and places them in a queue that is accessible 
			by all the three threads A, B and C. 

			For each pair of input (X,Y), we require that the request must first be processed by 
			thread A (Multiplication (X*Y)),
			then B(Division (X*Y)/2),
			then C (Addition (X+Y)), 
			then B again ( (X*Y) /(X+Y)), and 
			finally by A((X+Y)* (X+Y))
			before the pair (X,Y) can be removed and discarded from the queue.

			Thread A must read the next request from the queue only after it is finished with all the above steps of 
			the previous one. 

			Write down code for the functions run by the threads A, B, and C, to enable this synchronization.
			You may use any synchronization primitive of your choice to solve this question.

			Example:

					Input: N, number of random pairs

			Output Format:

					Initial Queue :

						Pairs  | A | B | C |   B   | A
						------------------------------
						(1, 2) | 2 | 1 | 3 | 0.667 | 9
    Solution :

       		1) First of all three semaphore variables are created and initialized for three threads.
	   	  		The semaphore variable for threadA is initialized to 1 and the other two are initialized to 0.
			3) Then three threads are created threadA, threadB, threadC.
			4) All of the threads run a while loop inside themselves. Inside every while loop a sem_wait() is called on their
			   corresponding semaphores. Because of these, in the first iteration of the wihile loops threadB and threadC
			   blocks because the semaphoreB and semaphoreC was initialised to 0. But thread A continues to execute.
			5) threadA checks the 'previousThread' variable to get the name of last executed thread. As threadA itself was the first
			   thread to run last time so 'previousThread' is set to 'o'. If statemnts make relevent instructions to run.
			   At the end, the 'previousThread' is set 'a' and sem_post() is called on 'semaphoreB' and threadB 
			   starts executing.
			   On the other hand while loop of threadA continues and 'threadA' calls sem_wait() again on semaphoreA
			   causing it to become blocked, because semaphoreA was decremented to 0 due to the previous sem_wait() call.
			6) Execution of the other threads continue in the same way.
			7) When the 'threadA' again becomes unblocked next time the 'previousThread' variable is set to 'b'.
				So, the relevent instructions are executed and in the end the (x, y) 'Pair' is deleted from the queue
				using the delete() function call, the 'previousThread' variable is set to 'o' and sem_post(semaphoreA)
				is called so that threadA can continue execution after it gets back to the begining of the loop and executes
				sem_wait(semaphoreA)
			8) This continues untill the queue becomes empty.
			9) When the queue beomes empty the isEmpty() call returns 1 and 'threadA' stops exectuing the while loop.
			   After that it calls sem_post(semaphoreB) and sem_post(semaphoreC) to unblock threadB and threadC which
			   were blocked during the previous execution due to the call to sem_wait() on their corresponding semaphores.
			10) At this moment the 'previousThread' variable is set to 'o' due to the last execution of threadA.
				When threadB and threadC starts executing again only the if block that checks for the emptiness of the
				queue is executed because the queue is now empty.
				The only instructions in those if blocks breaks the threads out of the while loops.
			11) After finishing execution all the three threads return causing the pthread_join() calls to return and the
				main() function also returns.
			
***********************************************************************************************************************

***********************************************************************************************************************
    INPUT DESCRIPTION : 
			1) Input can be given using commandline arguments. Number of arguments must be even.
				Execution Sequence : ./19_8 x y a b m n p q ...

			2) Input can also be given after the program has started execution and prompts to input an option : 

				1.Insert a Pair
				2.Display Queue
				3.Process Queue
				Enter your choice : 1
				Enter x <space> y: 12 34
			
			3) Choosing option 2 while the program prompts for a choice will display the all the elements of the queue.
			4) Choosing option 3 will make thr program to process the contents of the queue using the threads.

    OUTPUT DESCRIPTION : 

        1) 

***********************************************************************************************************************

***********************************************************************************************************************
    EXECUTION :

        Compilation Command     :  gcc 19_8.c -o 19_8 -pthread
        Execution Sequence      :  ./19_8 x y a b m n ... or,
									./19_8
									(arguments are optional but must in even numbers when provided)

***********************************************************************************************************************

***********************************************************************************************************************
        
    SAMPLE INPUT  : ./19_8 1 2 8 13 12 21 102 21 141 233 1245 327 1451 3659 14523 1251

    SAMPLE OUTPUT :

			# 8 pairs inserted through Commandline

			1.Insert a Pair
			2.Display Queue
			3.Process Queue
			Enter your choice : 1
			Enter x <space> y: 145 24

			1.Insert a Pair
			2.Display Queue
			3.Process Queue
			Enter your choice : 2
			The queue is :
			        x       |       y       |
			----------------------------------
			        1       |       2       |
			        8       |       13      |
			        12      |       21      |
			        102     |       21      |
			        141     |       233     |
			        1245    |       327     |
			        1451    |       3659    |
			     14523      |       1251    |
			        145     |       24      |

			1.Insert a Pair
			2.Display Queue
			3.Process Queue
			Enter your choice : 3

			Pairs           |       A       |       B       |       C       |       B       |       A       |
			-------------------------------------------------------------------------------------------------
			(1, 2)          |       2       |     1.000     |       3       |     0.667     |       9       |
			(8, 13)         |       104     |     52.000    |       21      |     4.952     |       441     |
			(12, 21)        |       252     |     126.000   |       33      |     7.636     |       1089    |
			(102, 21)       |       2142    |     1071.000  |       123     |     17.415    |     15129     |
			(141, 233)      |     32853     |     16426.500 |       374     |     87.842    |     139876    |
			(1245, 327)     |     407115    |    203557.500 |       1572    |     258.979   |     2471184   |
			(1451, 3659)    |     5309209   |   2654604.500 |       5110    |     1038.984  |     26112100  |
			(14523, 1251)   |     18168273  |   9084136.000 |     15774     |     1151.786  |     248819076 |
			(145, 24)       |       3480    |     1740.000  |       169     |     20.592    |     28561     |

***********************************************************************************************************************
*/

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
void* threadA(void* args);	/*function for thread A*/
void* threadB(void* args);	/*function for thread B*/
void* threadC(void* args);	/*function for thread C*/

void printColumnIntger(int number);   	//for printing the 'number' integer in a column of the output table
void printColumnFloat(float number);	//for printing 'number' in a column of the output table
void printPair(int x, int y); 			//for print x and y as (x, y) in a column of the output table

void processCommandLineInputs(int argc, char **argv); //this function reads the commandline inputs and inserts them in the queue as (x, y) pairs

int main(int argc, char **argv)
{
	//handle commandline arguments
	if((argc-1) % 2 != 0)
	{
		printf("\nIncorrect input. Values must be inserted in paires.\nEven number of arguments required.\n");
		printf("Execution Command : ./19_8 x y x y x y x y ....\n\n");
		exit(1);
	}
	processCommandLineInputs(argc, argv);

	//handle interactive interface
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


	//start processing the queue
    if(isEmpty())
	{
		printf("Queue is empty. Nothing to process...\n\n");
		exit(0);
	}
	//creating thread id variables to hold thread ids
    pthread_t idA;
    pthread_t idB;
    pthread_t idC;
    
    //initialising semaphore variables for synchronising threads
	if(sem_init(&semaphoreA, 0, 1) == -1)
		perror("Error initializing semaphoreA ");
	if(sem_init(&semaphoreB, 0, 0) == -1)
		perror("Error initializing semaphoreB ");
	if(sem_init(&semaphoreC, 0, 0) == -1)
		perror("Error initializing semaphoreC ");

	//print labels for labeling execution results
	printf("Pairs\t\t|\tA\t|\tB\t|\tC\t|\tB\t|\tA\t|\n");
	for(int i=0; i<97; i++)printf("-");

    /*creating threads*/
    int errnoA = pthread_create(&idA,NULL,&threadA,NULL);
	if(errnoA != 0)
    {
        printf("Error creating threadA : %s\n", strerror(errnoA));
       	exit(1); 
    }
    
    int errnoB = pthread_create(&idB,NULL,&threadB,NULL);
	if(errnoB != 0)
    {
        printf("Error creating threadA : %s\n", strerror(errnoB));
       	exit(1); 
    }
    
    int errnoC = pthread_create(&idC,NULL,&threadC,NULL);
	if(errnoC != 0)
    {
        printf("Error creating threadA : %s\n", strerror(errnoC));
       	exit(1); 
    }
    
	//waiting for child threads to finish
	errnoA = pthread_join(idA,NULL);
	if(errnoA != 0)
    {
        printf("Error joining threadA : %s\n", strerror(errnoA));
       	exit(1); 
    }
	errnoB = pthread_join(idB, NULL);
	if(errnoB != 0)
    {
        printf("Error joining threadB : %s\n", strerror(errnoB));
       	exit(1); 
    }
    errnoC = pthread_join(idC, NULL);
	if(errnoC != 0)
    {
        printf("Error joining threadC : %s\n", strerror(errnoC));
       	exit(1); 
    }

    return 0;
}


void* threadA(void* args)
{
	while (!isEmpty()) /*run until queue is empty*/
	{
        int x,y, result;
		if(sem_wait(&semaphoreA) == -1)
		{
			perror("Error calling sem_wait on semaphoreA ");
			exit(2);
		}
		
		if (previousThread == 'o')
		{
			dequeue(&x, &y);
			printPair(x, y);

			result = x*y;
			printColumnIntger(result);

			previousThread = 'a';
			
			if(sem_post(&semaphoreB) == -1)
				perror("Error calling sem_post on semaphoreB ");
		}
		else if (previousThread == 'b')
		{
            dequeue(&x, &y);
			result = ((x+y) * (x+y));
            printColumnIntger(result);

            delete();

			previousThread = 'o';

			if(sem_post(&semaphoreA) == -1)
				perror("Error calling sem_post on semaphoreA ");
		}
	}
	printf("\n\n");

	//sem_post is called to unblock threadA and threadB
	if(sem_post(&semaphoreB) == -1)
		perror("Error calling sem_post on semaphoreB ");
	if(sem_post(&semaphoreC) == -1)
		perror("Error calling sem_post on semaphoreC ");
}


void* threadB(void* args)
{
	while (1)
	{
		if(sem_wait(&semaphoreB) == -1)
		{
			perror("Error calling sem_wait on semaphoreB ");
			exit(2);
		}
        int x, y;
		float result;
		if (previousThread == 'a')
		{
            dequeue(&x, &y);
			result = (float)((x*y)/2.0);
			printColumnFloat(result);

			previousThread = 'b';

			if(sem_post(&semaphoreC) == -1)
				perror("Error calling sem_post on semaphoreC ");
		}
		else if (previousThread == 'c')
		{
            dequeue(&x, &y);
			result = (float)(x*y/((float)x+(float)y));
			printColumnFloat(result);

			previousThread = 'b';

			if(sem_post(&semaphoreA) == -1)
				perror("Error calling sem_post on semaphoreA ");
		}
        if(isEmpty())
            break;
	}
}


void* threadC(void* args)
{
	while (1)
	{
		if(sem_wait(&semaphoreC) == -1)
		{
			perror("Error calling sem_wait on semaphoreC ");
			exit(2);
		}

        int x, y, result;
		if (previousThread == 'b')
		{
			dequeue(&x, &y);
			result = x+y;
			printColumnIntger(result);
        
			previousThread = 'c';

			if(sem_post(&semaphoreB) == -1)
				perror("Error calling sem_post on semaphoreB ");
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
	else if(strlen(bufferX)+strlen(bufferY) >= 4 )
		printf("\n(%d, %d)\t|", x, y);
	else
		printf("\n(%d, %d)\t\t|", x, y);
}