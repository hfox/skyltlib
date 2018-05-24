/*
 *		skylten.h
 */

#ifndef SKYLT_H
#define SKYLT_H

#define BYTESPERBANK		512
#define BITSPERPIC		2304
#define BYTESPERPIC		288
#define BYTESPERLINE 	12
#define LINES				24

#define SKYLT_X			96
#define SKYLT_Y			24

/*
 *		Operating modes to use with set_mode()
 */
#define BS_MODE_ROM             0
#define BS_MODE_RAM1            1		/* 1 bit color */
#define BS_MODE_RESERVED        2
#define BS_MODE_RAM4            3		/* 4 bit color */

#ifdef __cplusplus
extern "C" {
#endif

long long unsigned int	init_skylt(void);
void		shutdown_skylt(long long unsigned int handle);
char		*get_skyltmem(long long unsigned int handle);
void		set_mode(long long unsigned int handle,int mode);
void		set_bank(long long unsigned int handle,int bank);
int			get_bank(long long unsigned int handle);


#ifdef __cplusplus
}
#endif

#endif//SKYLT_H
