#ifndef __PORTABLE_TIMER__
#define __PORTABLE_TIMER__

/*
	(c) 2000, Anton Ragnarsson
	huvudsakligen skriven för att kunna göra program
	för basenskylten på olika plattformar
*/


#ifdef __cplusplus
extern "C" {
#endif


int    init_timer(void);		/* anropas i början */
void   reset_timer(void);		/* nollställer timern */
float  get_timer(void);			/* ger tiden i sekunder */
void   snooze(int ms);			/* vänta en viss tid */


#ifdef __cplusplus
}
#endif

#endif//__PORTABLE_TIMER__
