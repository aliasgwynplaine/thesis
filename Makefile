CC = gcc
NVCC = nvcc
SRCFILES = pol.c matrix.c tree.c cli.c outils.c

OBJ = $(SRCFILES:.c=.o)

CPUFLAGS = -g -D_DEBUG -Wall
GPUFLAGS = -D_DEBUG -Wall
LDFLAGS = -lm
INC_DIRS = inc
SRC_DIR  = src
INC_FLAGS = $(addprefix -I , $(INC_DIRS))

CLISRC = $(addprefix $(SRC_DIR)/, $(SRCFILES))

MAINTEST  = tests/main_tests.c
TESTFILES = pol.c tree.c matrix.c outils.c
TESTSRC   = $(addprefix $(SRC_DIR)/, $(TESTFILES))


PROGRAM = main

tests.exe: outils.o tree.o matrix.o pol.o cuda_test.o
	$(CC) $(MAINTEST) outils.o tree.o matrix.o pol.o cuda_test.o -o $@ $(INC_FLAGS) -g $(LDFLAGS)

parser.exe: src/main_parser.c grammar.o lexer.o parser.o
	$(CC) -g -Wall grammar.o lexer.o main.o -lfl -o parser.exe $(INC_FLAGS)

outils.o:
	$(CC) -c $(SRC_DIR)/outils.c -o $@ $(INC_FLAGS) 

tree.o:
	$(CC) -c $(SRC_DIR)/tree.c -o $@ $(INC_FLAGS)

matrix.o:
	$(CC) -c $(SRC_DIR)/matrix.c -o $@ $(INC_FLAGS)

pol.o:
	$(CC) -c $(SRC_DIR)/pol.c -o $@ $(INC_FLAGS) -lm

cli:
	$(CC) $(CLISRC) $(CPUFLAGS) -o $@.out $(INC_FLAGS) $(LDFLAGS)

cuda_test.o:
	$(NVCC) -c $(INC_DIRS)/cuda_test.cu -o cuda_test.o

grammar.o: src/grammar.c
	$(CC) -c src/grammar.c -o $@ $(INC_FLAGS)

lexer.o: src/lexer.c 
	$(CC) -c src/lexer.c -o lexer.o -lfl $(INC_FLAGS)

parser.o: src/main_parser.c
	$(CC) $(CFLAGS) -c src/main_parser.c -o main.o $(INC_FLAGS)

src/grammar.c: src/grammar.y
	bison -d -v src/grammar.y
	mv src/grammar.tab.c src/grammar.c
	mv src/grammar.tab.h inc/grammar.h

src/lexer.c: src/lexer.l
	flex src/lexer.l
	mv lex.yy.c src/lexer.c

.PHONY: clean
clean :
	rm $(PROGRAM) $(OBJ)

.PHONY: clean_parser
clean_parser:
	rm -f parser.exe lexer.o grammar.o src/grammar.c inc/grammar.h lex.yy.c