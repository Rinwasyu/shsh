.PHONY: all clean

CC := gcc
CFLAGS := -Wall -O3

all: shsh

shsh: shsh.o wildcard.o builtin.o command.o filenames.o
	$(CC) -o $@ $^

shsh.o: shsh.c
	$(CC) $(CFLAGS) -c $^

wildcard.o: wildcard.c
	$(CC) $(CFLAGS) -c $^

builtin.o: builtin.c
	$(CC) $(CFLAGS) -c $^

command.o: command.c
	$(CC) $(CFLAGS) -c $^

filenames.o: filenames.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f shsh *.o
