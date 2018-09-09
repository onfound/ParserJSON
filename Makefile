CC=gcc
CFLAG=-pedantic -Wall -Wextra
CFLAGS=$(CFLAG) -c -o

Parser_JSON : main.o parser.o
        $(CC) $(CFLAGS) -o Parser_JSON main.o parser.o

main.o : Main.c
        $(CC) $(CFLAGS) main.o Main.c

parser.o : Parser.c
        $(CC) $(CFLAGS) parser.o Parser.c