#include <stdio.h>
#include <assert.h>

#include "version.h"
#include "mem.h"
#include "ast.h"
#include "bytecode.h"

SkNode *sk_bytecode_parse_number(FILE *stream) {
    SkNodeNumber *node = sk_malloc(sizeof(SkNodeNumber));
    node->type = SK_NODE_TYPE_NUMBER;
    assert(fread(&node->value, sizeof(int), 1, stream) == 1);
    return (SkNode *)node;
}

SkNode *sk_bytecode_parse_nil(FILE *stream) {
    SkNode *node = sk_malloc(sizeof(SkNode));
    node->type = SK_NODE_TYPE_NIL;
    return node;
}

SkNode *sk_bytecode_parse_message(FILE *stream) {
    SkNodeMessage *node = sk_malloc(sizeof(SkNodeMessage));
    node->type = SK_NODE_TYPE_MESSAGE;
    /* read: 
     * the name
     * the receiver
     * the arguments
     */
    node->name = sk_bytecode_parse_node(stream); // check?
    node->receiver = sk_bytecode_parse_node(stream); // check
    /* read the argument count */
    assert(fread(&node->argcount, sizeof(int), 1, stream) == 1);
    node->arguments = sk_malloc(sizeof(SkNode *) * node->argcount);
    /* read the arguments! */
    int i;
    for(i=0; i < node->argcount; i++) {
        node->arguments[i] = sk_bytecode_parse_node(stream);
    }
    return (SkNode *)node;
}

SkNode *sk_bytecode_parse_string(FILE *stream) {
    SkNodeString *node = sk_malloc(sizeof(SkNodeString));
    node->type = SK_NODE_TYPE_STRING;
    int length;
    assert(fread(&length, sizeof(int), 1, stream) == 1);
    char string[length+1]; /* uh? */
    assert(fread(&string, sizeof(char), length, stream) == length);
    // mhm, terminate it. TODO: we should not use char* here.
    string[length] = '\0';
    node->value = bfromcstr(string);
    return (SkNode *)node;
}

SkNode *sk_bytecode_parse_block(FILE *stream) {
    int i; 
    SkNodeBlock *node = sk_malloc(sizeof(SkNodeBlock));
    node->type = SK_NODE_TYPE_BLOCK;
    assert(fread(&node->count, sizeof(int), 1, stream) == 1);
    node->children = sk_malloc(sizeof(SkNode *));
    for(i = 0; i < node->count; i++) {
        node->children[0] = sk_bytecode_parse_node(stream);
    }
    return (SkNode *)node;
}

SkNode *sk_bytecode_parse_filename(const char *filename) {
    FILE *file = fopen(filename, "rb");
    assert(file != NULL);
    SkNode *root = sk_bytecode_parse_stream(file);
    fclose(file);
    return root;
}

SkNode *sk_bytecode_parse_stream(FILE *stream) {
    /* check signature. the first four bytes of the file
     * have to be 'sk' + the major bytecode version + the
     * minor bytecode version (as ordinal values)
     */
    char *sig = sk_malloc(sizeof(char) * 4);
    assert(fread(sig, sizeof(char), 4, stream) == 4);
    if(!(sig[0] == 's' && sig[1] == 'k' 
                && sig[2] == SK_BYTECODE_VERSION_MAJOR 
                && sig[3] == SK_BYTECODE_VERSION_MINOR)) {
        printf("The bytecode signature is not supported.\n");
        return NULL;
    }
    /* now, parse */
    return sk_bytecode_parse_node(stream);
    /* left over stuff is left over */
}

SkNode *sk_bytecode_parse_node(FILE *stream) {
    int type;
    assert(fread((void *)&type, sizeof(int), 1, stream) == 1);
    switch(type) {
        case SK_NODE_TYPE_NUMBER:
            return sk_bytecode_parse_number(stream);
        case SK_NODE_TYPE_BLOCK:
            return sk_bytecode_parse_block(stream);
        case SK_NODE_TYPE_STRING:
            return sk_bytecode_parse_string(stream);
        case SK_NODE_TYPE_MESSAGE:
            return sk_bytecode_parse_message(stream);
        case SK_NODE_TYPE_NIL:
            return sk_bytecode_parse_nil(stream);
        default:
            printf("Error: " __FILE__ ": No idea what node type %d is.\n", type);
            return NULL;
    }
}
