/*
 *
 */

#define FONTWIDTH		1024
#define FONTHEIGHT	8
#define CHARWIDTH		7

extern void load_font(char *filename);
extern void put_char(int x1, int y1, char c, int col);
extern void put_string(int x1, int y1, char* s, int col);
extern void anim_string(int x1, int y1, int x2, int y2, char *s, int length, int count, int col);
extern void anim_string_unwrapped(int x1, int y1, int x2, int y2, char *s, int length, int count, int col);
