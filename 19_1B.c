/*
***********************************************************************************************************************
   
                                           ***** OPERATING SYSTEMS LAB *****
*   TEAM NO : 19
*   TEAM MEMBERS :      KOUSHIK MAHANTA(L)     ( 002011001106 )
*                       DISHARI SAHA           ( 002011001107 )
            		TUSAR MUNDHRA          ( 002011001122 )
			DEBARGHA DATTA         ( 002011001082 )

***********************************************************************************************************************


    PROBLEM : Program to GET and SET environment variables using getenv() and setenv() system calls.


***********************************************************************************************************************

    EXECUTION :

    	Compilation Command	:  gcc 19_1B.c -o 19_1B
    	Execution Sequence	:  ./19_1B
	
	Sample Output :
               
        	USER = user
		HOME = /home/user
		HOST = localhost.localdomain
		ARCH = x86_64
		DISPLAY = :0
		No environment variable named 'PRINTER' exists
		PATH = /usr/local/bin:/usr/local/sbin:/usr/bin:/usr/sbin


***********************************************************************************************************************
*/

#include <stdlib.h>		//contains the declaration of the getenv() function
#include <stdio.h>		//for basic I/O and error handling
#include <unistd.h>		//contains the declaration of the gethostname() function
#include <sys/utsname.h>	//contains the definition of the structure utsname

#define MAX_LENGTH 10		

int main()
{
	char envVariableName[][MAX_LENGTH] = {{"USER"},{"HOME"},{"HOST"},{"ARCH"},{"DISPLAY"},{"PRINTER"},{"PATH"}};
	

	char hostname[50];
	
	//getting the host name and storing it in 'hostname' variable
    	if(gethostname(hostname,50)<0)
		perror("Error getting hostname");			

 	/*perror() uses the 'errno' variable and prints the error informationif error occures which is checked by the  		return value of the function Rreturn value 0 means success and -1 means error*/
	
	//setting a new environment variable named 'HOST'and setting its value to the value in hostname
    	if(setenv("HOST",hostname,1)<0)	
		perror("Error setting 'HOST' environment variable");


	//declaring a buffer to hold the structure returned by uname()
	struct utsname buf;
	
	//getting the utsname structure containing the system information in 'buf'
	if(uname(&buf)<0)
		perror("Error getting system informatin using uname");
		
	//setting a new environment variable named 'ARCH' and setting its value to the value in 'buf.machine'
	if(setenv("ARCH",buf.machine,1)<0)
        	perror("Error setting 'ARCH' environment variable");


	int i;
	//loop through the array 'envVariableName' to print there values using getenv() function
	for(i=0; i<7; i++)
	{
		//the getenv() function returns a pointer to the value in the environment or NULL if there is no match.
		//the pointer returned by getenv is assigned to 'envValue'
		char *envValue = getenv(envVariableName[i]);
		
		//if 'envValue' is NULL that means no match for that environment variable is found
		//it prints the error message
		if(envValue == NULL)
			printf("Error : no environment variable named '%s' exists\n",envVariableName[i]);

		//if 'envValue' is not NULL then it prints the value of the environment variable along with its name
		else
			printf("%s = %s\n",envVariableName[i],envValue);
	}
	return 0;
}
