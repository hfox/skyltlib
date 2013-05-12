
default : font.o graphics.o skyltreal.o timer.o

all : clean default

clean :
	rm *.o

.c.o : $<
	gcc -c $<
