/*
***********************************************************************************************************************
   
                                           ***** OPERATING SYSTEMS LAB *****

*   DATE : 03/11/2022
*   TEAM NO : 19
*   TEAM MEMBERS :      KOUSHIK MAHANTA(L)     ( 002011001106 )
                       	DISHARI SAHA           ( 002011001107 )
            		TUSAR MUNDHRA          ( 002011001122 )
		        DEBARGHA DATTA         ( 002011001082 )

***********************************************************************************************************************

    PROBLEM :	The objective of this assignment is to use thread programming technique to
		write a parallel program which will find the output of an edge detection filter.
		The input to the filter is a rectangular matrix (MxN), in which the elements
		are the pixel values of an image. The output is also a rectangular matrix of
		same size where the elements are the edge pixel values. The edge pixel
		values are constructed from the image using:-

		edge i,j = image i−1,j + image i+1,j + image i,j−1 + image i,j+1 − 4 image i,j

		If an image pixel has the same value as its four surrounding neighbors (i.e.
		no edge) then the value of edge i,j will be zero. If the pixel is very different
		from its four neighbors (i.e. a possible edge) then edge i,j will be large in
		magnitude. We will always consider i and j to lie in the range 1, 2, . . .M and
		1, 2, . . .N respectively. Pixels that lie outside this range (e.g. image i,0 or
		image M+1,j ) are simply considered to be set to zero.

		You have to also measure the time elapsed. The Matrix should be large
		enough (dynamically allocated) to fit in your computer memory so as to
		make the speedup measurement process easy.

		As you increase the number of threads(max to your number of CPUs), your
		timing and speedup should show improvement. Use ‘gettimeofday’ to note
		down the timing. Make sure you attach the timing to the starting comment
		block of your source file and also show the speedup. Your time should not
		include the Matrix initialization time.

		You have to use various ‘Pthread’ library calls to do this assignment
		properly. Use the proper process listing command (ps) to show (and attach
		it to the starting comment block of your source file) that your threads are
		running in the system. Also, use proper command to show CPU and
		Memory utilization and attach these outputs to the starting comment block
		of your source file.

		This program should take five command line arguments. The First
		argument is the Row Size of the Rectangular Matrix. The Second argument
		is the Column Size of the Rectangular Matrix.The Third argument is about the number of threads. 
		For example, the value of the second argument will be 1, 2, 3, and 4 etc.; signifying total no of
		threads which will be created.

		The Fourth argument will be the value of the ‘mod’ which will be used to
		initialize all the elements of the input Rectangular Matrix.

		The Fifth (last) argument will be the print_switch. If its value is ‘1’, both the
		Input and Output Matrices will be printed on the screen. In case the value is
		‘0’, Matrices won’t be printed on the screen.

***********************************************************************************************************************

    EXECUTION :

    	Compilation Command	:  gcc 19_9.c -o 19_9 pthread
    	Execution Sequence	:  ./19_9 3 3 2 7 1
	
	Sample Output :
	Time Required = 0.000002 

		Matrix  : 

			3 3 6 
			6 0 2 
			4 2 6 
		Result : 

			-3 -3 -19 
			-17 13 4 
			-8 2 -20 				       
   
***********************************************************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include <time.h>
#include <sys/random.h>
#include <sys/time.h>
#define max_thread 1000


int Count_thread = 0;
int m = 0;
int n = 0;
int mod = 0;
int print_switch = 0 ;

int **matrix;      // stores the original matrix.
int **result;           //  stores the output edge pixel matrix.
float totalTime = 0;   // stores the total time.


int** matrixCreate(int m, int n);
void output(int **arr);


void* edge(void *arg)
{
    struct timeval start, end;  // Timer.

    gettimeofday(&start, NULL);  // Timer started.
  

    long int thread_num = (long int)arg;
    int i , j ,k ;

    int Start = (thread_num * m)/Count_thread;
    int goal = ((thread_num+1)*m) /Count_thread;

    for(int i = Start ; i < goal ; i++)
    {
        for(int j = 0 ; j < n ; j++)
        {
            result[i][j] = 0;
            int left=0,right=0,top=0,bottom=0;
            if(i!=0)
                top = matrix[i-1][j];
            if(i!=m-1)
                bottom=matrix[i+1][j];
            if(j!=0)
                left=matrix[i][j-1];
            if(j!=n-1)
                right=matrix[i][j+1];
            
            result[i][j] = top + left + right + bottom - (4*matrix[i][j]);
        }
    }

    gettimeofday(&end, NULL); // Timer stopped.
  
    double time_required;   // Calculates the total time required by the program.
  
    time_required = (end.tv_sec - start.tv_sec) * 1e6;
    time_required = (time_required + (end.tv_usec - start.tv_usec)) * 1e-6;
    totalTime += time_required;

} 

int main(int argc, char *argv[]){
    
    if( argc != 6)
    {
        printf(" Input valid arguments!!\n");
        exit(0);
    }

    m = atoi(argv[1]);
    n = atoi(argv[2]);
    Count_thread = atoi(argv[3]);
    mod = atoi(argv[4]);
    print_switch = atoi(argv[5]);
    
    pthread_t tid[max_thread];
    long int i , j;

    matrix = matrixCreate(m,n);
    result = matrixCreate(m,n);

  
    for(i = 0 ; i < Count_thread ; i++)  // Creation of threads.
    {
        if (pthread_create(&tid[i], NULL, &edge, (void *)i) != 0)
	    {
		    perror("Could not create Thread\n");
		    exit(0);
	    }
    }


    for(j = 0 ; j < Count_thread ; j++)  // Joining the threads.
    {
        if (pthread_join(tid[j], NULL) != 0)
	    {
		    perror("Could not join Thread \n");
		    exit(0);
        }
    }

  
    printf("Time Required = %f \n",totalTime);

    if(print_switch == 1)
    {
        printf("\nMatrix  : \n");
        output(matrix);
        printf("Result : \n");
        output(result);
    }

    free(matrix);
    free(result);

    return 0;
}

void output(int **arr)
{
    int i , j;
    int r = m;
    int c = n;
     for (i = 0; i < r; i++)
     {
        printf("\n");
        for (j = 0; j < c; j++)
            printf("%d ", arr[i][j]);
     }
     printf("\n");
}

int** matrixCreate(int m, int n)       
{
    int r = m, c = n, i, j;
 
    int** array = (int**)malloc(r * sizeof(int*));
    for (i = 0; i < r; i++)
        array[i] = (int*)malloc(c * sizeof(int));
 
    
    for (i = 0; i < r; i++)
        for (j = 0; j < c; j++)
        {
            // Random Number Generator
            int random_Number ;
            if (getrandom(&random_Number, sizeof(int), GRND_NONBLOCK) == -1)
	        {
		        perror("getrandom():");
		        exit(1);
	        }
            if(random_Number < 0 )
            {
                random_Number *= -1;
            }
            array[i][j] = random_Number % mod;
	}
	return array;
}
