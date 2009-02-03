#include <stdio.h>
#include <assert.h>

#include "object.h"
#include "number.h"
#include "message.h"
#include "skstring.h"
#include "list.h"
#include "block.h"
#include "ast.h"

SkObject *sk_ast_evaluate_block(SkVM *vm, SkNode *node) {
    SkObject *self = sk_block_clone(sk_vm_get_proto(vm, "Block"));
    /* set the initial value */
    sk_block_set_block(self, (SkNodeBlock *)node);
    return self;
}

SkObject *sk_ast_evaluate_message(SkVM *vm, SkNode *node_) {
    SkNodeMessage *node = (SkNodeMessage *)node_;
    SkObject *self = sk_message_clone(sk_vm_get_proto(vm, "Message"));
    sk_message_set_name(self, sk_ast_evaluate_string(vm, node->name));
    SkObject *receiver = sk_ast_evaluate(vm, node->receiver);
    if(receiver == vm->nil) {
        receiver = vm->lobby;
    }
    sk_message_set_receiver(self, receiver);

    SkObject *args = sk_message_clone(sk_vm_get_proto(vm, "List"));
    int i;
    for(i = 0; i < node->argcount; i++) {
        SkObject *argument = sk_ast_evaluate_message(vm, node->arguments[i]);
        sk_list_append(args, argument);
    }

    return sk_object_dispatch_message(receiver, self);
}

SkObject *sk_ast_evaluate_string(SkVM *vm, SkNode *node) {
    SkObject *self = sk_string_clone(sk_vm_get_proto(vm, "String"));
    /* set the initial value */
    sk_string_set_bstring(self, ((SkNodeString *)node)->value);
    return self;
}

/* clone and return a number */
SkObject *sk_ast_evaluate_number(SkVM *vm, SkNode *node) {
    SkObject *self = sk_number_clone(sk_vm_get_proto(vm, "Number")); 
    /* set the initial value */
    sk_number_set_int(self, ((SkNodeNumber *)node)->value);
    return self;
}

SkObject *sk_ast_evaluate(SkVM *vm, SkNode *node) {
    switch(node->type) {
        case SK_NODE_TYPE_NUMBER:
            return sk_ast_evaluate_number(vm, node);
        case SK_NODE_TYPE_BLOCK:
            return sk_ast_evaluate_block(vm, node);
        case SK_NODE_TYPE_STRING:
            return sk_ast_evaluate_string(vm, node);
        case SK_NODE_TYPE_MESSAGE:
            return sk_ast_evaluate_message(vm, node);
        case SK_NODE_TYPE_NIL:
            return vm->nil;
        default:
            printf("Error: " __FILE__ ": No idea what node type %d is.\n", node->type);
            return NULL;
    }
}
