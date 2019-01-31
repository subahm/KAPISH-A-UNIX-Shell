CC = gcc
CFLAGS = -std=c11 -Wall -Werror

kapish: kapish.o
	$(CC) $(CFLAGS) kapish.c -o kapish

kapish.o: kapish.c
	$(CC) $(CFLAGS) kapish.c -o kapish.o

clean:
	-rm kapish.o kapish 