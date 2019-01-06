CC = gcc
CFLAGS = -pedantic -Wall -Wextra -Werror -ldb -fsanitize=address -ggdb
objects =  aNode.o earl.o

default: $(objects)
	$(CC) $(CFLAGS) $(objects) -o earl

