/*
***********************************************************************************************************************
   
                                           ***** OPERATING SYSTEMS LAB *****
*   DATE : 22/09/2022
*   TEAM NO : 19
*   TEAM MEMBERS :      KOUSHIK MAHANTA(L)     ( 002011001106 )
                        DISHARI SAHA           ( 002011001107 )
                        TUSAR MUNDHRA          ( 002011001122 )
                        DEBARGHA DATTA         ( 002011001082 )

***********************************************************************************************************************

***********************************************************************************************************************


    ASSIGNMENT DETAILS :    Assignment 6

    Problem : The OS is a program that uses various data structures. Like all programs in 
execution, you can determine the performance and other behaviour of the OS by
 inspecting its state - the values stored in its data structures. In this part of the 
assignment, we study some aspects of the organization and behaviour of a Linux 
system by observing values of kernel data structures exposed through the /proc 
virtual file system.



The /proc virtual file system:


Linux uses the /proc file system to collect information from kernel data structures.
The /proc implementation provided with Linux can read many different kernel data
structures. If you cd to /proc on a Linux machine, you will see a number of files and 
directories at that location. Files in this directory subtree each correspond to some 
kernel data structure. The subdirectories with numeric names contain virtual files
with information about the process whose process ID is the same as the directory
name. Files in /proc can be read like ordinary ASCII files. You can open each file
and read it using library routines such as fgets() or fscanf(). The proc (5) manual 
page explains the virtual files and their content available through the /proc file
system.



Requirements in detail:


You are supposed to write a C program which should run continuously and print lists
of the following dynamic values every seconds:


	? Per second disk read/write in the system

	? Per second Timer Interrupts

	? Currently executing kernel scheduling entities (processes, threads)

	? Number of kernel scheduling entities that currently exist on the system




#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

FILE*fp;

#define MAXBUFFLEN 1000
unsigned char* buf[MAXBUFFLEN];
int buflen=0;

void main()
{
    	int i=0;
	while(1)
	{
		sleep(1);

		fp=fopen("/proc/interrupts","rb");
  		if(fp!=NULL)
  		{
 			i = 0;
    			while(!feof(fp))
    			{
      
			buf[i] = (char*) malloc(500);
			fscanf(fp, "%[^\n] ", buf[i]);
			i++;
    			} 
	
			printf("%s\n",buf[15]);
	
  		} 
		free(buf[15]);
		fclose(fp);

	}
}

