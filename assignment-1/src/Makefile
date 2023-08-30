CC = gcc
CFLAGS = -g

OBJ = lex.o parser.o main.o

.SUFFIXES: .c .l .y .o

all: lex.yy.c main

lex.yy.c: lex.l
	flex lex.l

parser.tab.c: parse.y
	bison -d parser.y

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIBS) -o $@

clean:
	rm -f *.yy.c *.tab.c $(OBJ) main > /dev/null 2>&1
