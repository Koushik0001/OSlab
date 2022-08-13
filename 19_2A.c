/*
***********************************************************************************************************************
   
                                           ***** OPERATING SYSTEMS LAB *****
*   DATE : 06/08/2022
*   TEAM NO : 19
*   TEAM MEMBERS :      KOUSHIK MAHANTA(L)     ( 002011001106 )
                        DISHARI SAHA           ( 002011001107 )
                        TUSAR MUNDHRA          ( 002011001122 )
                        DEBARGHA DATTA         ( 002011001082 )

***********************************************************************************************************************

	ASSIGNMENT DETAILS : Assignment 2A

    Problem : Catch the signal 'SIGINT' and dispaly "Ha Ha, Not Stopping". Use 'signal' system call.
	
	Solution : 

		1) In the main() function signal() sets the disposition of the SIGINT to SignalHandler()
			that means whenever a SIGINT signal is sent to the process the SignalHandler() is invoked
			to handle the signal.
		2) An infinite while loop for simulating a long running process
		3) sleep(1) is used to prevent the process from consuming the most of the CPU
		4) When the process is run it keeps on printing "Process Running"
			and when 'Ctrl+c' is pressed a SIGINT signal is sent to the process causing the SugnalHandler()
			function to take control
		5) SignalHandler() function checks to see if the signum is set to SIGINT and if true prints 
			"SIGINT caught, Ha Ha, Not Stopping."
		6) To stop the process from running press 'Ctrl+\', this will kill the process.


***********************************************************************************************************************

***********************************************************************************************************************
    INPUT DESCRIPTION : No Input Required

    OUTPUT DESCRIPTION : 

        1) This program when executed keeps on printing "Process Running"
        2) When "Ctrl+c" is pressed it prints "SIGINT caught. Ha, Ha, Not Stopping."

***********************************************************************************************************************

***********************************************************************************************************************

    EXECUTION :

        Compilation Command     :  gcc 19_2A.c -o 19_2A
        Execution Sequence      :  ./19_2A

***********************************************************************************************************************

***********************************************************************************************************************
		SAMPLE INPUT : No Input Required.

        SAMPLE OUTPUT :

			Process Running...
			Process Running...
			Process Running...
			Process Running...
			^C
			SIGINT caught. Ha Ha, Not Stopping.
			Process Running...
			Process Running...
			Process Running...
			Process Running...
			^C
			SIGINT caught. Ha Ha, Not Stopping.
			Process Running...
			Process Running...
			Process Running...
			Process Running...
			^\Quit (core dumped)
               
                
***********************************************************************************************************************
*/
#include <stdio.h> 	//for basic I/O functionalities
#include <signal.h>	//for handling signals
#include <errno.h>	//for handling  errors
#include <unistd.h>	//for sleep() system call

//whenever a SIGINT signal is sent to the process, this function is invoked to handle the signal 
void SignalHandler(int signum)
{
	if(signum == SIGINT)
    	printf("\nSIGINT caught. Ha Ha, Not Stopping.\n");
}
int main()
{
	//signal() sets the disposition of the SIGINT to SignalHandler()
    if(signal(SIGINT, SignalHandler) == SIG_ERR)
    	perror("\nError, SIGINT not caught :");


	//infinite while loop for simulating a long running process
    while(1)
    {
        printf("Process Running...\n");
                
		//sleep is used to prevent the process from consuming the most of the CPU
		//this introduces a one second delay between each consecutive while loops
		sleep(1);
    }

	return 0;
}
