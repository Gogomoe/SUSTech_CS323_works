CC=gcc
CXX=g++
CXXFLAGS:=-std=c++17
FLEX=flex
BISON=bison


.lex: lex.l
	$(FLEX) lex.l
.syntax: syntax.y
	$(BISON) -v -t -d syntax.y
splc: .lex .syntax
	mkdir -p bin && $(CXX) $(CXXFLAGS) syntax.tab.c ASTNode.cpp SemanticAnalyzer.cpp -lfl -ly -o bin/splc
clean:
	@rm -f lex.yy.c syntax.tab.* *.out *.so
all: splc