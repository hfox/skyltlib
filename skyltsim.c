/*
 *		skyltsim.c
 */

#define SKYLT_C

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
#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <errno.h>
#include "skylten.h"

#define X_SIZE	SKYLT_X*8
#define Y_SIZE	SKYLT_Y*8

struct skylt
{
	Display			*display;
	Window			window;
	int				screen;
	Atom				protocol[1];
	int				depth;
	unsigned long	black,color[16];
	unsigned int	*imagebuf;
	XImage			*image;
	char				skyltmem[BYTESPERBANK*4];
	pthread_t			update_thread;
	int				update_delay;
	int				running;
	int				mode;
	int				bank;
};

/*
 *		Write image to skylt window
 */

void put_image(struct skylt *s,char *image)
{
	unsigned long	c;
	int				x,y,e=0;
	char				*p;
	unsigned int		*d;
	static int print=-1;
//	print++;
// rita ut alla lysdioder

	#define XY ((y)*8*X_SIZE+x*8)
	#define R0 X_SIZE*0
	#define R1 X_SIZE*1
	#define R2 X_SIZE*2
	#define R3 X_SIZE*3
	#define R4 X_SIZE*4
	#define R5 X_SIZE*5
	#define R6 X_SIZE*6
	#define R7 X_SIZE*7
	#define CURPIX (p[(y*SKYLT_X + x)/8] & (1 << (7-x%8)))

	p = image;
	d = s->imagebuf;
//	for(y=0;y<SKYLT_Y;y++,d+=8*X_SIZE)
	if(print%10==0 && print) printf("\x1b[25F");
	if(print%10==0) printf("%p\n-----------------------\n", (void*)s->image->width);
//	for(y=SKYLT_Y;y>0;--y)
	for(y=0;y<SKYLT_Y;y++)
	{
//		if (print) printf("d=%d\n", d);
//		for(x=0;x<12;x++,p++)
		for(x=0;x<SKYLT_X;x++)
		{
//			for(b=7;b>=0;b--,d+=8)
//			for(b=7;b>=0;b--)
			{
//				c = s->color[(*p & (1 << b)) ? 7 : 0];
				c = CURPIX ? s->color[0] : s->color[7];
				if (print%10==0) putchar( CURPIX ? ' ' : '0');
//				c = (unsigned long int)16711680;
/*int j,k;
	for(j=0; j<8; j++)
		for(k=0; k<8; k++) {
			XPutPixel(s->image, x*8+j, y*8+k, c);*/

#if 1
//							d[R1+2]=d[R1+3]=d[R1+4]=d[R1+5]         = c;
							d[XY+R1+2]=d[XY+R1+3]=d[XY+R1+4]=d[XY+R1+5]				= c;
//				d[R2+1]=	d[R2+2]=d[R2+3]=d[R2+4]=d[R2+5]=d[R2+6] = c;
				d[XY+R2+1]=	d[XY+R2+2]=d[XY+R2+3]=d[XY+R2+4]=d[XY+R2+5]=d[XY+R2+6]	= c;
//				d[R3+1]=	d[R3+2]=d[R3+3]=d[R3+4]=d[R3+5]=d[R3+6] = c;
				d[XY+R3+1]=	d[XY+R3+2]=d[XY+R3+3]=d[XY+R3+4]=d[XY+R3+5]=d[XY+R3+6]	= c;
//				d[R4+1]=	d[R4+2]=d[R4+3]=d[R4+4]=d[R4+5]=d[R4+6] = c;
				d[XY+R4+1]=	d[XY+R4+2]=d[XY+R4+3]=d[XY+R4+4]=d[XY+R4+5]=d[XY+R4+6]	= c;
//				d[R5+1]=	d[R5+2]=d[R5+3]=d[R5+4]=d[R5+5]=d[R5+6] = c;
				d[XY+R5+1]=	d[XY+R5+2]=d[XY+R5+3]=d[XY+R5+4]=d[XY+R5+5]=d[XY+R5+6]	= c;
//							d[R6+2]=d[R6+3]=d[R6+4]=d[R6+5]         = c;
							d[XY+R6+2]=d[XY+R6+3]=d[XY+R6+4]=d[XY+R6+5]				= c;
//				d[XY+R2+4] = c;
#endif
			}
		}
		if(print%10==0) putchar('\n');
	}
//	XPutImage(s->display, s->window, s->gc, s->image, 0,0, 0,0, X_SIZE,Y_SIZE);
	printf("                                                                                                                                   (%02x,%02x)", d[0], d[(SKYLT_Y-1)*8*X_SIZE+(SKYLT_X-1)*8+X_SIZE*7+5]);
	e = XPutImage(s->display, s->window, XDefaultGC(s->display, s->screen), s->image, 0,0, 0,0, X_SIZE,Y_SIZE);
	printf(" error. %d\r", e);
	XFlush(s->display);
}

/*
 *		Main function for updating thread
 */

void *update_main(void *arg)
{
	XEvent			xevent;
	struct skylt	*s = (struct skylt *)arg;
	char			membuf[BYTESPERBANK];

	// Töm eventkön

	while(s->running)
	{
		while (XPending(s->display))
		{
			XNextEvent(s->display, &xevent);
			switch (xevent.type)
			{
				case ClientMessage:
					if ((Atom)xevent.xclient.data.l[0]==s->protocol[0])
						exit(0);
					break;
			}
		}

		memcpy(membuf, (s->skyltmem)+(s->bank*BYTESPERBANK), BYTESPERBANK);

		put_image(s,membuf);
		usleep(s->update_delay);
	}
	return(0);
}

/*
 *		Initialize the skylt
 */

long long unsigned int init_skylt(void)
{
	struct skylt	*s;
	XSizeHints		size_hints;
	XWMHints			wm_hints;
	XClassHint		class_hint;
	XTextProperty	window_name;
	Colormap			cmap;
	XColor			c;
	Visual			*visual;
	int				/*x,y,o,w,*/i;
	char 				*title = "Basen-skylt simulatorfönster knyckt av Anton Ragnarsson";

	if((s = (struct skylt *)malloc(sizeof(struct skylt))))
	{
		memset(s,0,sizeof(struct skylt));
		if ((s->display = XOpenDisplay(NULL)))
		{
			s->screen = DefaultScreen(s->display);
			XSynchronize(s->display, 1);
// fixa lysdiod-färger
			cmap = DefaultColormap(s->display, s->screen);
			s->black = BlackPixel(s->display, s->screen);
			c.blue=c.green=0;
			for (i=0; i<8; i++)
			{
/*				c.red = (39+27*i)<<8;*/
				c.red = 2535+9000*i;
				XAllocColor(s->display, cmap, &c);
				s->color[i] = c.pixel;
			}
			for (i=8; i<16; i++)
				s->color[i]=s->color[7];
		
// fixa bitmap
			visual = DefaultVisual(s->display, s->screen);
			s->depth = DefaultDepth(s->display, s->screen);
		
			if((s->imagebuf = (unsigned int *)malloc(X_SIZE*Y_SIZE*(s->depth==8?1:4))))
			{
				printf("imagebuf: %p\n", (void*)s->imagebuf);
				memset(s->imagebuf,0,X_SIZE*Y_SIZE*(s->depth==8?1:4));
				s->image = XCreateImage(s->display,visual,s->depth,ZPixmap,0,(char*)s->imagebuf,X_SIZE,Y_SIZE,8,0);
			
				if (s->depth==8) {
					memset(s->imagebuf, s->black, X_SIZE*Y_SIZE);
					s->depth = 1;
				} else {
					memset(s->imagebuf, 0, X_SIZE*Y_SIZE*4);
					s->depth = 4;
				}
// fixa fönster   
				s->window = XCreateSimpleWindow(s->display,RootWindow(s->display,s->screen),
														100,10, X_SIZE, Y_SIZE, 5, s->black,s->black);
				
				XSelectInput(s->display, s->window, ExposureMask);
				XMapRaised(s->display, s->window);
				
				XStringListToTextProperty(&title, 1, &window_name);
				
				size_hints.flags       = PMinSize | PMaxSize;
				size_hints.min_width   = X_SIZE;
				size_hints.max_width   = X_SIZE;
				size_hints.min_height  = Y_SIZE;
				size_hints.max_height  = Y_SIZE;
				
				wm_hints.flags         = InputHint | StateHint;
				wm_hints.input         = True;
				wm_hints.initial_state = NormalState;
				
				class_hint.res_name    = "skyltenResource";
				class_hint.res_class   = "skyltenWindow";
				
				XSetWMProperties(s->display, s->window,
						&window_name, &window_name,
						NULL, 0,
						&size_hints, &wm_hints, &class_hint);
				
				s->protocol[0] = XInternAtom(s->display, "WM_DELETE_WINDOW", False);
				XSetWMProtocols(s->display, s->window, s->protocol, 1);
				
				//s->gc = XCreateGC(s->display, s->window, 0, 0);
				XCreateRegion();
// skylt-specifikt
				s->running = 1;
				s->update_delay = 10000;
// tråd
/*				if(!(i=thr_create(0,0,update_main,(void *)s,THR_DETACHED,&s->update_thread)))*/
				sleep(1);
				if(!(i=pthread_create(&s->update_thread,NULL,update_main,(void *)s))) {
					printf("skyltstruct: %#018llx\n", (long long unsigned int)s);
				} else {
					printf("Ingen tråd (%d)\n",i);
					//free(s);
					s = 0;
				}
			}
		} else
			puts("FEL: Kunde inte öppna DISPLAY");
	}
	return((long long unsigned int)s);
}

/*
 *		Shut down skylt
 */

void shutdown_skylt(long long unsigned int handle)
{
	struct skylt	*s = (struct skylt *)handle;

	s->running = 0;
	
	pthread_join(s->update_thread, NULL);

	sleep(1);
	
	printf("%p\n", (void*)s->imagebuf);
	//XFreeGC(s->display,s->gc);
	XDestroyWindow(s->display,s->window);
	XCloseDisplay(s->display);
	XDestroyImage(s->image);
//	free(s->imagebuf);
	free(s);
}

/*
 *		Get pointer to skylt memory
 */

char *get_skyltmem(long long unsigned int handle)
{
//	printf("skyltstruct: %#018llx\n", handle);
	return(((struct skylt *)handle)->skyltmem);
}

/*
 *		Set skylt mode
 */

void set_mode(long long unsigned int handle,int mode)
{
//	printf("skyltstruct: %#018llx\n", handle);
	((struct skylt *)handle)->mode = mode;
}

/*
 *		Set active bank
 */

void set_bank(long long unsigned int handle,int bank)
{
//	printf("skyltstruct: %#018llx\n", handle);
	((struct skylt *)handle)->bank = bank;
}

/*
 *		Get active bank
 */

int get_bank(long long unsigned int handle)
{
//	printf("skyltstruct: %#018llx\n", handle);
	return(((struct skylt *)handle)->bank);
}
/*
int main(void)
{
	int	handle,i,j,t;

	for(i=0;i<288;i++)
		skyltmem[i]=0x55;
	handle=init_skylt();
//	put_image(buf);
	for(j=1;j<1000;j++)
	{
		for(i=0;i<288;i++)
			skyltmem[i] = (j % (i+1));
		usleep(10000);
	}
	getchar();
	shutdown_skylt(handle);
}
*/
