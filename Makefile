CC = gcc
CFLAGS = -Wall -Wextra -std=c11

SRC_DIR = src
EXE_DIR = .
SRC = $(SRC_DIR)/initializer.c $(SRC_DIR)/finalizer.c $(SRC_DIR)/producer.c $(SRC_DIR)/spy.c
DEPENDENCIES = $(SRC_DIR)/sharedMemory.c $(SRC_DIR)/utilities.h
EXE = $(patsubst $(SRC_DIR)/%.c,$(EXE_DIR)/%.out,$(SRC))

all: $(EXE)

$(EXE_DIR)/%.out: $(SRC_DIR)/%.c $(DEPENDENCIES)
	$(CC) -o $@ $^

# Run 'make clean' to remove all .out files
clean:
	rm $(EXE_DIR)/*.out