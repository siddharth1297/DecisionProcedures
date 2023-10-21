CC    ?= clang
CXX   ?= clang++

OPT = -g -O0
#OPT = -O3

EXE = main

CDEBUG = -Wall

CXXDEBUG = -Wall

CSTD = -std=c11
CXXSTD = -std=c++14

CFLAGS = $(OPT) $(CDEBUG) $(CSTD) 
CXXFLAGS = $(OPT) $(CXXDEBUG) $(CXXSTD)


CPPOBJ = main fe_driver
SOBJ =  parser lexer

FILES = $(addsuffix .cpp, $(CPPOBJ))

OBJS  = $(addsuffix .o, $(CPPOBJ))

CLEANLIST =  $(addsuffix .o, $(OBJ)) $(OBJS) \
				 fe_parser.tab.cc fe_parser.tab.hh \
				 location.hh position.hh \
			     stack.hh fe_parser.output parser.o \
				 lexer.o fe_lexer.yy.cc $(EXE)\

.PHONY: all
all: wc

wc: $(FILES)
	$(MAKE) $(SOBJ)
	$(MAKE) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(OBJS) parser.o lexer.o $(LIBS)

#BISON=/opt/homebrew/opt/bison/bin/bison
BISON=bison
parser: fe_parser.yy
	$(BISON) -d -v fe_parser.yy
	$(CXX) $(CXXFLAGS) -c -o parser.o fe_parser.tab.cc

lexer: fe_lexer.l
	flex --outfile=fe_lexer.yy.cc  $<
	$(CXX)  $(CXXFLAGS) -c fe_lexer.yy.cc -o lexer.o

.PHONY: test
test:
	test/test0.pl

.PHONY: clean
clean:
	rm -rf $(CLEANLIST)

format:
	clang-format -i *.cpp *.hpp
