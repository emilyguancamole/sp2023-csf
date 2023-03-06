CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic

SRCS = csim.c
OBJS = $(SRCS:%.c=%.o)

all : csim

csim : $(OBJS)
	$(CC) -o $@ $(OBJS)

clean :
	rm -f $(OBJS) csim depend.mak

depend :
	$(CC) $(CFLAGS) -M $(SRCS) > depend.mak

depend.mak :
	touch $@

include depend.mak
