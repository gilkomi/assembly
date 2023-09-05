CC=gcc
CFLAGS=-g -fno-omit-frame-pointer -fsanitize=address -Wall -ansi -pedantic -c
LFLAGS=-g -fno-omit-frame-pointer -fsanitize=address -Wall -ansi -pedantic
OBJECTS=am_builder.o coded_list.o first_pass.o lexer.o main.o parser.o second_pass.o symbol_table.o utils.o
EXEC=assembler

$(EXEC): $(OBJECTS)
	$(CC) $(LFLAGS) $(OBJECTS) -o $(EXEC)

am_builder.o: am_builder.c am_builder.h utils.h
	$(CC) $(CFLAGS) am_builder.c

coded_list.o: coded_list.c coded_list.h lexer.h utils.h symbol_table.h
	$(CC) $(CFLAGS) coded_list.c

first_pass.o: first_pass.c first_pass.h symbol_table.h coded_list.h utils.h
	$(CC) $(CFLAGS) first_pass.c

lexer.o: lexer.c lexer.h utils.h parser.h
	$(CC) $(CFLAGS) lexer.c

main.o: main.c lexer.h am_builder.h symbol_table.h coded_list.h first_pass.h second_pass.h
	$(CC) $(CFLAGS) main.c

parser.o: parser.c parser.h lexer.h utils.h
	$(CC) $(CFLAGS) parser.c

second_pass.o: second_pass.c second_pass.h symbol_table.h coded_list.h utils.h
	$(CC) $(CFLAGS) second_pass.c

symbol_table.o: symbol_table.c symbol_table.h utils.h
	$(CC) $(CFLAGS) symbol_table.c

utils.o: utils.c utils.h lexer.h
	$(CC) $(CFLAGS) utils.c

clean:
	rm -f $(OBJECTS) $(EXEC)

