CC = gcc
CFLAGS = -pedantic -Wall -Wextra -Werror -ldb 
objects =  aNode.o earl.o
executable = earl

default: $(objects) aNode.h
	$(CC) $(CFLAGS) $(objects) -o $(executable)

debug: $(objects) aNode.h
	$(CC) $(CFLAGS) $(objects) -ggdb -fsanitize=address -o $(executable)

clean:
	rm *.o $(executable)
