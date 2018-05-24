/*
 *		font.c
 */

#include <stdio.h>
#include "global.h"
#include "graphics.h"
#include "font.h"

/*
 *		Font table
 */

char	font[FONTHEIGHT*FONTWIDTH];
int	font_table[256];

/*
 *		Load font
 */

void load_font(char *filename)
{
	FILE	*file;
	int	n;

	if(file = fopen(filename, "rb"))
	{
		fread(font,FONTHEIGHT*FONTWIDTH,1,file);

		for(n=0;n<256;n++)
			font_table[n] = -1;
		for(n = 'A'; n <= 'Z'; n++)
		{
			font_table[n] = CHARWIDTH*(n - 'A');
			font_table[n + 'a' - 'A'] = CHARWIDTH*(n - 'A' + 29);
		}
		font_table[' '] = CHARWIDTH*100;
		for(n = '0'; n <= '9'; n++)
			font_table[n] = 406 + CHARWIDTH*(n - '0');
		font_table['('] = 511;
		font_table[')'] = 518;
		font_table[':'] = 476;
		font_table['.'] = 476;
		font_table[','] = 483;
		font_table['!'] = 490;
		font_table['?'] = 497;
		font_table['å'] = 385;
		font_table['ä'] = 392;
		font_table['ö'] = 399;
		font_table['Å'] = 385;
		font_table['Ä'] = 392;
		font_table['Ö'] = 399;
		font_table['%'] = 504;
		font_table['~'] = 525;
		font_table['/'] = 532;
		font_table['\\'] = 539;
		font_table['-'] = 546;
		font_table['_'] = 553;
		font_table['+'] = 560;
		font_table['='] = 567;
		font_table['@'] = 574;
		font_table['#'] = 581;
		font_table['$'] = 588;
		font_table['"'] = 595;
		font_table['\''] = 602;
		font_table['['] = 609;
		font_table[']'] = 616;
		font_table['{'] = 623;
		font_table['}'] = 630;
		font_table['&'] = 637;
		font_table['<'] = 644;
		font_table['>'] = 651;
		font_table['|'] = 658;
		
		fclose(file);
	} else {
		printf("font open failed\n");
	}

}

/*
 *		Put chararacter
 */

void put_char(int x1, int y1, char c, int col)
{
	int	x,y,x_offs = font_table[c];
	char	*p;

	if (x_offs != -1)
		for(y = 0; y < FONTHEIGHT; y++)
			for(x = 0; x < CHARWIDTH; x++)
			{
				p = font + y*FONTWIDTH + x + x_offs;
				if(!*p)
					putpixel(x1+x,y1+(7-y),col);
			}
}

/*
 *		Put string
 */

void put_string(int x1, int y1, char *s, int col)
{
	char *p;
	int  x = x1;

	for(p = s; *p; p++, x+=CHARWIDTH)
		put_char(x,y1,*p,col); 
}

/*
 *		Animate string
 */

void anim_string(int x1, int y1, int x2, int y2, char *s, int length, int count, int col)
{
  int	width	= x2 - x1 + 1;
  int   len	= length*CHARWIDTH;
  int	n       = count % len;
  
  clip();
  set_cliprect(x1,y1,x2,y2);
  put_string(x1 - n, y1, s, col);
  put_string(x1 - n + len, y1, s, col);
  noclip();
 
}

/*
 *		Animate string, only once.
 */

void anim_string_unwrapped(int x1, int y1, int x2, int y2, char *s, int length, int count, int col)
{
  int	n       = count;
  
  clip();
  set_cliprect(x1,y1,x2,y2);
  put_string(x1 - n+60, y1, s, col);
  noclip();
 
}
