
default : font.o graphics.o skyltreal.o skyltsim.o timer.o

all : clean default

clean :
	rm *.o

.c.o : $<
	gcc -c $<

