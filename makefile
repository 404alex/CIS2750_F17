CC = gcc
CFLAGS = -Wall -g -I include -std=gnu11 -L bin -shared

all: list parser

list:
	$(CC) $(CFLAGS) -o bin/LinkedListAPIlib.so -fPIC src/LinkedListAPI.c
parser: list
	$(CC) $(CFLAGS) -o bin/helplib.so -fPIC src/help.c include/CalendarParser.h src/CalendarParser.c src/LinkedListAPI.c
	$(CC) $(CFLAGS) -o bin/parserlib.so -fPIC src/CalendarParser.c bin/helplib.so bin/LinkedListAPIlib.so
clean:
	rm bin/*
