CC = gcc

all: parser list
parser:
	$(CC) -Iinclude -c src/CalendarParser.c -o bin/CalendarParser.o
	ar cr bin/libcparse.a bin/CalendarParser.o
list: include/LinkedListAPI.h
	$(CC) -Iinclude -c src/LinkedListAPI.c -o bin/LinkedListAPI.o
	ar cr bin/libllist.a bin/LinkedListAPI.o
clean:
	rm bin/*.o bin/*.so