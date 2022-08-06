CC = gcc
SRC = $(wildcard src/*.c)
TESTSRC = $(wildcard tests/*.c)
OBJ = $(SRC:.c=.o)

CFLAGS = -g -02 -Wall
LDFLAGS = -lm
INC_DIRS = inc
INC_FLAGS = $(addprefix -I,$(INC_DIRS))

PROGRAM = main

matrix: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(INC_FLAGS)

.PHONY: clean
clean :
	rm $(PROGRAM) $(OBJ)