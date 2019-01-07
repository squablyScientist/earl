#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aNode.h"

/**
 * Creates a new alias tree node with no adjacencies and returns a pointer to
 * it.
 *
 * @param data: The name of the alias tree node to be created
 *
 * @returns: A pointer to the newly created 
 */
aNode* newAliasNode(char* data){
	// TODO check that mallocs were successful
	aNode* new = calloc(1, sizeof(aNode));
	strncpy(new->name, data, MAX_USRNAME_LEN);
	new -> adjacencies = malloc(sizeof(aNode*) * ADJ_UNIT_SIZE);
	new -> numAdj = 0;
	new -> size = ADJ_UNIT_SIZE;
	return new;
}


/**
 * Marks the adjacency src->dst. Does this by inserting dst to the end of the
 * src's adjacency array. Since it has the potential to resize the array w/
 * realloc, this is an O(n) operation, but is amortized to an O(1) operation
 * since resizing is so rare.
 *
 * @param src: The src node of the adjacency to be marked
 * @param dst: The dst node of the adjacency to be marked
 *
 * @returns nothing.
 */
void addAdjacency(aNode* src, aNode* dst){
	
	/*
	 * Checks if the max number of adjacencies is reached and reallocs more
	 * memory to allow for more adjacencies and increases the max adjacencies.
	 */
	if(src -> numAdj >= src -> size){
		src -> size += ADJ_UNIT_SIZE;
		src -> adjacencies = realloc(src -> adjacencies, 
				sizeof(aNode*)* (src -> size));
	}
	src -> adjacencies[(src -> numAdj)++] = dst;
}
