CC = gcc
NVCC = nvcc
SRCFILES = pol.c matrix.c tree.c cli.c outils.c

OBJ = $(SRCFILES:.c=.o)

CFLAGS = -g -D_DEBUG
LDFLAGS = -lm
INC_DIRS = inc
SRC_DIR  = src
INC_FLAGS = $(addprefix -I , $(INC_DIRS))

CLISRC = $(addprefix $(SRC_DIR)/, $(SRCFILES))

MAINTEST  = tests/main_tests.c
TESTFILES = pol.c tree.c matrix.c outils.c
TESTSRC   = $(addprefix $(SRC_DIR)/, $(TESTFILES))


PROGRAM = main

cli:
	$(CC) $(CLISRC) $(CFLAGS) -o $@.out $(INC_FLAGS) $(LDFLAGS)

matrix: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(INC_FLAGS)

.PHONY: tests
tests:
	$(CC) $(MAINTEST) $(TESTSRC) -o $@.out $(INC_FLAGS) -g $(LDFLAGS)

.PHONY: clean
clean :
	rm $(PROGRAM) $(OBJ)