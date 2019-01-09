// TODO: find a way to free the forest after it's no longer of any use
#include <stdio.h>
#include <alloca.h>
#include <string.h>
#include <sys/types.h>
#include <db.h>
#include <search.h>
#include <unistd.h>
#define _GNU_SOURCE

#include "aNode.h"

#define DATABASE "names.db"
#define BUFFER_SIZE 256
#define USERNAME_GIVEN 0x1
#define FILENAME_GIVEN 0x2

/**
 * Std usage w/ a custom message
 */
void usage(char* msg){
	fprintf(stderr, "%s\n"
			"Usage: earl -i <db filename> -u <username / alias>\n", msg);
}

int main(int argc, char** argv)
{


	int option;
	char* usrname;
	char* dbFilename;

	// 8 bits for the options that could be given. 
	unsigned char optionsGiven = 0;

	while((option = getopt(argc, argv, "i:u:")) != -1){
		switch(option){
			case 'i':
				dbFilename = optarg;
				optionsGiven |= FILENAME_GIVEN;
				break;
			case 'u':
				usrname = optarg;
				optionsGiven |= USERNAME_GIVEN;
				break;
			case '?':
				return 1;
		}
	}

	if(!(optionsGiven & FILENAME_GIVEN)){
		usage("filename of aliases db needed");
		return 1;
	}

	if(!(optionsGiven & USERNAME_GIVEN)){
		usage("username / alias needed");
		return 1;
	}

	// DB struct pointer that will represent the alias DB
	DB* aliases;

	// Cursor struct pointer that will represent the cursor looking through the
	// db, acts as a way to enumerate all pairings that exist in the DB
	DBC* cursor;

	// Allocates mem for and reads in the DB from the file
	db_create(&aliases, NULL, 0);
	aliases->open(aliases, NULL, dbFilename, NULL, DB_HASH, 0, 0);

	// Allocates and binds the cursor to the DB
	aliases->cursor(aliases, NULL, &cursor, 0);

	// Allocates mem for the data returned and used in queries. 
	char* keydata = alloca(BUFFER_SIZE);
	char* valdata = alloca(BUFFER_SIZE);

	// Declaraes a key and a val DBT for use with DB queries and enumeration
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

	// Creates a hash table for finding users that have already been seen
	// before.
	hcreate(1000);

	// ENTRYs for hsearch entry insertions and query.
	ENTRY src;
	ENTRY dst;
	dst.key = alloca(MAX_USRNAME_LEN);
	dst.data = alloca(sizeof(aNode));
	src.key = alloca(MAX_USRNAME_LEN);
	src.data = alloca(sizeof(aNode));

	// Buffer to hold tokens for multivalue alias entries e.g. vapehouse
	char* token;

	// Iterates over every record in the db, recording all adjacencies.
	while(cursor->get(cursor, &key, &val, DB_NEXT) == 0){

		// Sets up strtok and parses the first token in the aliases
		token = strtok((char*)val.data, " ,");

		// Performs this loop on each token in the aliases
		do{
			strncpy(src.key, token, MAX_USRNAME_LEN);
			strncpy(dst.key, (char*)key.data, MAX_USRNAME_LEN);

			// Creates and inserts the src node if it doesn't already exist
			if(hsearch(src, FIND) == NULL){
				src.data = newAliasNode(src.key);
				hsearch(src, ENTER);
			}

			// Creates and inserts the dst node if it doesn't already exist
			if(hsearch(dst, FIND) == NULL){
				dst.data = newAliasNode(dst.key);
				hsearch(dst, ENTER);
			}
			
			// Marks these two nodes as adjacent to eachother(directed)
			aNode* srcNode = (aNode*)hsearch(src,FIND)->data;
			aNode* dstNode = (aNode*)hsearch(dst,FIND)->data;
			addAdjacency(srcNode, dstNode);

			// Makes new memory for the keys since the intial memory must be
			// preserved for hsearch to work properly.
			dst.key = alloca(MAX_USRNAME_LEN);
			src.key = alloca(MAX_USRNAME_LEN);
		}
		while((token = strtok(NULL, " ,")));
	}

	// Finds the aNode of the root of the tree(the user / alias being queried
	// about)
	ENTRY rootEntry;
	rootEntry.key = usrname;
	aNode* root = (aNode*)(hsearch(rootEntry, FIND)->data);

	// Prints the end username for referece in the output
	printf("%s: ", usrname);

	
	if(root != NULL){

		// Prints out a list of all the aliases that direct to root
		for(size_t i = 0; i < root->numAdj; i++){

			// This loop "flattens" the tree, making all neighbors' neighbors of
			// root direct neighbors of root. Since those neighbors will then be
			// examined this simulates recursion. This is the only use for the
			// tree in the first place, and thus it is ok that this is a
			// destructive operation.
			for(size_t j = 0; j < root->adjacencies[i]->numAdj; j++){
				addAdjacency(root, root->adjacencies[i]->adjacencies[j]);
			}
			printf("%s ", root->adjacencies[i]->name);
		}
	}
	putchar('\n');


	// Frees the memory associated with the DB and it's cursor, also closes the
	// open fd of the underlying DB file.
	cursor->close(cursor);
	aliases->close(aliases, 0);

	// Frees the memory that is associated with the structure of the hsearch map
	hdestroy();

	return 0;
}

