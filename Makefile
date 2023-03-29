CC = gcc
NVCC = nvcc
SRCFILES = pol.c matrix.c tree.c cli.c outils.c sym_table.c

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

tests.exe: outils.o tree.o matrix.o pol.o sym_table.o
	$(CC) $(MAINTEST) outils.o tree.o matrix.o pol.o sym_table.o -o $@ $(INC_FLAGS) -g $(LDFLAGS)

parser.exe: $(SRC_DIR)/main_parser.c grammar.o lexer.o parser.o
	$(CC) -g -Wall grammar.o lexer.o main.o -lfl -o parser.exe $(INC_FLAGS)

sym_table.o: $(SRC_DIR)/sym_table.c
	$(CC) -c $(SRC_DIR)/sym_table.c -o $@ $(INC_FLAGS)

outils.o: $(SRC_DIR)/outils.c
	$(CC) -c $(SRC_DIR)/outils.c -o $@ $(INC_FLAGS) 

tree.o: $(SRC_DIR)/tree.c
	$(CC) -c $(SRC_DIR)/tree.c -o $@ $(INC_FLAGS)

matrix.o: $(SRC_DIR)/matrix.c
	$(CC) -c $(SRC_DIR)/matrix.c -o $@ $(INC_FLAGS)

pol.o: $(SRC_DIR)/pol.c
	$(CC) -c $(SRC_DIR)/pol.c -o $@ $(INC_FLAGS) -lm

cuda_test.o:
	$(NVCC) -c $(INC_DIRS)/cuda_test.cu -o cuda_test.o

grammar.o: $(SRC_DIR)/grammar.c
	$(CC) -c $(SRC_DIR)/grammar.c -o $@ $(INC_FLAGS)

lexer.o: $(SRC_DIR)/lexer.c 
	$(CC) -c $(SRC_DIR)/lexer.c -o lexer.o -lfl $(INC_FLAGS)

parser.o: $(SRC_DIR)/main_parser.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main_parser.c -o main.o $(INC_FLAGS)

src/grammar.c: $(SRC_DIR)/grammar.y
	bison -d -v $(SRC_DIR)/grammar.y -o grammar.c 
	mv grammar.c $(SRC_DIR)/grammar.c
	mv grammar.h inc/grammar.h

src/lexer.c: $(SRC_DIR)/lexer.l
	flex $(SRC_DIR)/lexer.l
	mv lex.yy.c $(SRC_DIR)/lexer.c

.PHONY: clean
clean :
	rm $(PROGRAM) $(OBJ)

.PHONY: clean_parser
clean_parser:
	rm -f parser.exe lexer.o grammar.o src/grammar.c src/grammar.output inc/grammar.h lex.yy.c src/lexer.c