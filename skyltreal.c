/*
 *		skyltreal.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <string.h>

#define BSRESET         0x100   /* reset the board */
#define BSSETBANK       0x101   /* set active memory page */
#define BSGETBANK       0x102   /* get active memory page */
#define BSSETMODE       0x103   /* set active mode (rom/1-bit ram/4-bit ram) */
#define BSGETMODE       0x104   /* get active mode */
#define BSSETRAW        0x105   /* set raw io */
#define BSGETRAW        0x106   /* get raw io */
#define BSCURADDR       0x107   /* query current memory address */
#define BSVERSION       0x108   /* read card version */

/*
 *		Initialize the skylt
 */

int init_skylt(void)
{
	return(open("/dev/bs0",O_RDWR));
}

/*
 *		Shut down skylt
 */

void shutdown_skylt(int handle)
{
	close(handle);
}

/*
 *		Get pointer to skylt memory
 */

char *get_skyltmem(int handle)
{
	return((char *)mmap(0,2048,PROT_READ | PROT_WRITE,MAP_SHARED,handle,0));
}

/*
 *		Set skylt mode
 */

void set_mode(int handle,int mode)
{
	unsigned char	n = (unsigned char)mode;
	
	ioctl(handle,BSSETMODE,&n);
}

/*
 *		Set active bank
 */

void set_bank(int handle,int bank)
{
	unsigned char n = (unsigned char)bank;

	ioctl(handle,BSSETBANK,&n);
}

/*
 *		Get active bank
 */

int get_bank(int handle)
{
	unsigned char bank;

	ioctl(handle,BSGETBANK,&bank);
	return (int)bank;
}
