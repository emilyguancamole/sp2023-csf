CC = g++
CFLAGS = -std=c++11 -g -Wall -Wextra -pedantic

SRCS = csim.c
OBJS = $(SRCS:%.c=%.o)

csim: main.o functions.o
	$(CC) -o csim main.o functions.o

main.o: main.cpp functions.h
	$(CC) -c main.cpp $(CFLAGS)

functions.o: functions.cpp functions.h
	$(CC) -c functions.cpp $(CFLAGS)

clean:
	rm -f *.o csim *~ *.gch

.PHONY: solution.zip
solution.zip :
	rm -f solution.zip
	zip -9r $@ Makefile *.h *.cpp