CC = gcc
CFLAGS = -Wall -g -I include -std=gnu11 -L bin

all: list parser clean

list:
	$(CC) $(CFLAGS) -c src/LinkedListAPI.c -o bin/LinkedListAPI.o
	ar cr bin/libllist.a bin/LinkedListAPI.o
parser: list
	$(CC) $(CFLAGS) -c src/help.c -o bin/help.o
	$(CC) $(CFLAGS) -c src/CalendarParser.c -o bin/CalendarParser.o
	ar cr bin/libcparse.a bin/CalendarParser.o bin/help.o bin/LinkedListAPI.o
clean:
	rm bin/*.o bin/*.so
