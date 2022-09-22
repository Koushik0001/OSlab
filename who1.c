/*
 * =====================================================================================
 *
 *       Filename:  who1.c
 *
 *    Description: First version of the who program
 *
 *        Version:  1.0
 *        Created:  06/11/2022 08:27:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Koushik Mahanta (KM), mahantakoushik80@gmail.com
 *   Organization:  Jadavpur University
 *
 * =====================================================================================
 */

#include<stdio.h>
#include<stdlib.h>
#include<utmpx.h>
#include<fcntl.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/uio.h>

#define SHOWHOST

void show_info(struct utmpx *utbufp);

int main()
{
    struct utmpx    *current_record;

    while((current_record = getutxent())!= NULL)
        show_info(current_record);
    return 0;
}

void show_info(struct utmpx *utbufp)
{
    printf("%s", utbufp->ut_user);
    printf(" ");
    printf("%s", utbufp->ut_line);
    printf(" ");
    printf("%f", utbufp->ut_tv.tv_sec);
    printf(" ");
    
#ifdef SHOWHOST
    printf("(%s)", utbufp->ut_host);
#endif
    printf("\n");
}
