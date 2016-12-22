
CC= gcc
CFLAGS= -Wall -pedantic

OBJS= opcode.o parser.o parser_callback.o

all: $(OBJS)


clean:
	rm -f $(OBJS)

test:
	gcc CFLAGS -c test.c -o test
	gcc CFLAGS $(OBJS) test.o -o test
	./test

.PHONY:all clean test