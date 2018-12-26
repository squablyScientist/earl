CC = gcc
CFLAGS = -pedantic -Wall -Wextra -Werror -ldb
default: earl.o
	$(CC) $(CFLAGS) earl.o -o earl
