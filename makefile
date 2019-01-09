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
install:
	cp earl /usr/local/bin 
	cp earl.1 /usr/local/share/man/man1
	mandb
uninstall:
	rm /usr/local/bin/earl 
	rm /usr/local/share/man/man1/earl.1
	mandb
