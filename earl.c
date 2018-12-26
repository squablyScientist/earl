// TODO: make matching handle multivalued values like vapehouse
#include <stdio.h>
#include <alloca.h>
#include <string.h>
#include <sys/types.h>
#include <db.h>
#include <getopt.h>

#define DATABASE "names.db"
#define BUFFER_SIZE 256

// TODO: allow this to recursively search for the names, rather than only the
// top level of aliasing. e.g. if teddy -> wheels -> squables, earl squables
// should print out:
// 		squables: teddy wheels
// rather than only reporting the wheels alias.
void printAliases(char* redirect, DBT* key, DBT* val, DBC* cursor)
{
	// Iterates over every record in the db
	while(cursor->get(cursor, key, val, DB_NEXT) == 0){
		if(!strcmp(redirect, (char*)val->data)){
			printf(" %s", (char*)key->data);
		}
	}
}

int main(int argc, char** argv)
{
	DB* aliases;
	DBC* cursor;
	db_create(&aliases, NULL, 0);
	aliases->open(aliases, NULL, DATABASE, NULL, DB_HASH, 0, 0);
	aliases->cursor(aliases, NULL, &cursor, 0);

	char* keydata = alloca(BUFFER_SIZE);
	char* valdata = alloca(BUFFER_SIZE);

	DBT key;
	DBT val;

	// Zeroes out the DBT structs so minimal work can be done to set them up
	memset(&key, 0, sizeof(DBT));
	memset(&val, 0, sizeof(DBT));

	// Sets up the key DBT to hold the incoming aliases
	key.data = keydata;
	key.ulen = BUFFER_SIZE;
	key.flags = DB_DBT_USERMEM;

	// Sets up the value DBT to hold the incoming redirects.
	val.data = valdata;
	val.ulen = BUFFER_SIZE;
	val.flags = DB_DBT_USERMEM;

	printf("%s:", argv[1]);

	printAliases(argv[1], &key, &val, cursor);

	putchar('\n');
	cursor->close(cursor);
	aliases->close(aliases, 0);

	(void)argv;
	(void)argc;
	return 0;
}

