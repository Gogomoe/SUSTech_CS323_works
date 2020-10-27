CC=gcc
FLEX=flex
BISON=bison


.lex: lex.l
	$(FLEX) lex.l
.syntax: syntax.y
	$(BISON) -v -t -d syntax.y
all: .lex .syntax
	$(CC) syntax.tab.c -lfl -ly -o json.out
clean:
	@rm -f lex.yy.c syntax.tab.* *.out