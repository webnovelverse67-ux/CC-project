CC = gcc
CFLAGS = -Wall -g

# Bison and Flex
BISON = bison
FLEX = flex

TARGET = mycompiler

all: $(TARGET)

lex.yy.c: lexer.l
	$(FLEX) lexer.l

parser.tab.c parser.tab.h: parser.y
	$(BISON) -d parser.y

$(TARGET): lex.yy.c parser.tab.c ast.c symtab.c tac.c semantic.c codegen.c main.c
	$(CC) $(CFLAGS) lex.yy.c parser.tab.c ast.c symtab.c tac.c semantic.c codegen.c main.c -o $(TARGET) -lfl

clean:
	rm -f lex.yy.c parser.tab.c parser.tab.h $(TARGET) target_code.out
