CC = gcc
CFLAGS = -Wall -Wextra -g
LDLIBS = -lm

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

ztop: $(OBJ)
	$(CC) $(OBJ) -o ztop $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o ztop

