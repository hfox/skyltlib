/*
 *      timer_UNIX.c
 */

#include <time.h>
#include <unistd.h>

#include "timer.h"


static clock_t start_time = -1;


int init_timer(void)
{
	if ((start_time = clock()) == -1)
		return 0;
	else
		return 1;
}



void reset_timer(void)
{
	start_time = clock();
}



float get_timer(void)
{
	return (clock() - start_time) / (float)CLOCKS_PER_SEC;
}



void snooze(int ms)
{
	usleep(1000 * ms);
}

