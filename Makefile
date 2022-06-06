# -*- MakeFile -*-

CFLAGS = -O0 -g3 -pedantic -pedantic-errors -Wall -Wextra -Werror
CC = gcc

main: main.c webserver.o
	$(CC) $(CFLAGS) $^ -o $@

webserver.o: webserver.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm main *.o