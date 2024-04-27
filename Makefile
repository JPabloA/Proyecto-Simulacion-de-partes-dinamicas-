CC = gcc

SRC = initializer.c finalizer.c
EXE = $(SRC:.c=.out)

all: $(EXE)

%.out: %.c
	$(CC) -o $@ $<
