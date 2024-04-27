CC = gcc

SRC_DIR = src
EXE_DIR = .
SRC = $(SRC_DIR)/initializer.c $(SRC_DIR)/finalizer.c
EXE = $(patsubst $(SRC_DIR)/%.c,$(EXE_DIR)/%.out,$(SRC))

all: $(EXE)

$(EXE_DIR)/%.out: $(SRC_DIR)/%.c
	$(CC) -o $@ $<