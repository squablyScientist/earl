#ifndef _ANODE_DEF_
#define _ANODE_DEF_
// This is the number of adjacencies initially allowed, increasing the allocated
// memory by this much each time the previous max amount is exceeded. 
#define ADJ_UNIT_SIZE 20
#define MAX_USRNAME_LEN 33

// Functions as a node of the tree itself
typedef struct aNode{
    char name[MAX_USRNAME_LEN];      // The name of the account/alias
                        // Array of the adjacencies of this node
    struct aNode** adjacencies;
    volatile size_t numAdj;      // The number of adjacencies of this node
    size_t size;        // The current max adjacencies
} aNode;

aNode* newAliasNode(char* data);
void addAdjacency(aNode* src, aNode* dst);
#endif
