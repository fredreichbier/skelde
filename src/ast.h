#ifndef AST_H_INCLUDED
#define AST_H_INCLUDED

#include "vm.h"

enum SkNodeType {
    SK_NODE_TYPE_NONE,
    SK_NODE_TYPE_BLOCK,
    SK_NODE_TYPE_NUMBER,
    SK_NODE_TYPE_STRING,
    SK_NODE_TYPE_MESSAGE,
    SK_NODE_TYPE_NIL,
};

#define SK_NODE_BASE \
    int type; /* a SkNodeType */

typedef struct _SkNode {
    SK_NODE_BASE;
} SkNode;

typedef struct _SkNodeBlock {
    SK_NODE_BASE;
    int count;
    SkNode **children;
} SkNodeBlock;

typedef struct _SkNodeNumber {
    SK_NODE_BASE;
    int value;
} SkNodeNumber;

typedef struct _SkNodeString {
    SK_NODE_BASE;
    bstring value;
} SkNodeString;

typedef struct _SkNodeMessage {
    SK_NODE_BASE;
    SkNode *name;
    SkNode *receiver;
    int argcount;
    SkNode **arguments;
} SkNodeMessage;

SkObject *sk_ast_evaluate_block(SkVM *vm, SkNode *node);
SkObject *sk_ast_evaluate_number(SkVM *vm, SkNode *node);
SkObject *sk_ast_evaluate_message(SkVM *vm, SkNode *node);
SkObject *sk_ast_evaluate_string(SkVM *vm, SkNode *node);
SkObject *sk_ast_evaluate(SkVM *vm, SkNode *node);

#endif
