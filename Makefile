CC = gcc
NVCC = nvcc
SRCFILES = pol.c matrix.c tree.c outils.c sym_table.c

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

parser.exe: grammar.o lexer.o main_parser.o pol_parser.o sym_table.o pol.o outils.o
	$(CC) -g -Wall grammar.o lexer.o main_parser.o pol_parser.o sym_table.o pol.o outils.o -lm -lfl -o parser.exe $(INC_FLAGS)

pol_parser.o: $(SRC_DIR)/$(@:.o=.c)
	$(CC) -c $(SRC_DIR)/$(@:.o=.c) -o $@ $(INC_FLAGS) -g

sym_table.o: $(SRC_DIR)/$(@:.o=.c)
	$(CC) -c $(SRC_DIR)/$(@:.o=.c) -o $@ $(INC_FLAGS) -g

outils.o: $(SRC_DIR)/$(@:.o=.c)
	$(CC) -c $(SRC_DIR)/$(@:.o=.c) -o $@ $(INC_FLAGS) -g

tree.o: $(SRC_DIR)/$(@:.o=.c)
	$(CC) -c $(SRC_DIR)/$(@:.o=.c) -o $@ $(INC_FLAGS) -g

matrix.o: $(SRC_DIR)/$(@:.o=.c)
	$(CC) -c $(SRC_DIR)/$(@:.o=.c) -o $@ $(INC_FLAGS) -g

pol.o: $(SRC_DIR)/$(@:.o=.c)
	$(CC) -c $(SRC_DIR)/$(@:.o=.c) -o $@ $(INC_FLAGS) -lm -g

cuda_test.o:
	$(NVCC) -c $(SRC_DIR)/$(@:.o=.cu) -o $@

grammar.o: src/grammar.c
	$(CC) -c $(SRC_DIR)/$(@:.o=.c) -o $@ $(INC_FLAGS) -g 

lexer.o: src/lexer.c
	$(CC) -c $(SRC_DIR)/$(@:.o=.c) -o $@ -lfl $(INC_FLAGS) -g

main_parser.o: $(SRC_DIR)/$(@:.o=.c)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/$(@:.o=.c) -o $@ $(INC_FLAGS) -g

src/grammar.c: $(SRC_DIR)/grammar.y
	bison -d -v $(SRC_DIR)/grammar.y -o grammar.c --verbose --debug
	mv grammar.c $(SRC_DIR)/grammar.c
	mv grammar.h inc/grammar.h

src/lexer.c: $(SRC_DIR)/lexer.l
	flex $(SRC_DIR)/lexer.l
	mv lex.yy.c $(SRC_DIR)/lexer.c

.PHONY: clean
clean :
	rm tests.exe $(OBJ)

.PHONY: clean_parser
clean_parser:
	rm -f parser.exe lexer.o grammar.o src/grammar.c src/grammar.output inc/grammar.h lex.yy.c src/lexer.c

.PHONY: clean_all
clean_all: clean_parser clean