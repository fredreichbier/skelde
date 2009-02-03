#include <stdio.h>

#include "vm.h"
#include "ast.h"
#include "object.h"
#include "number.h"
#include "list.h"
#include "message.h"
#include "block.h"
#include "skstring.h"
#include "bytecode.h"

int main(int argc, char** argv) {
    SkVM *vm = sk_vm_new();
    // The Object proto is added implictly.
    sk_vm_add_proto(vm, "Number", sk_number_create_proto(vm));
    sk_vm_add_proto(vm, "Block", sk_block_create_proto(vm));
    sk_vm_add_proto(vm, "List", sk_list_create_proto(vm));
    sk_vm_add_proto(vm, "Message", sk_message_create_proto(vm));
    sk_vm_add_proto(vm, "String", sk_string_create_proto(vm));

    SkObject *foo = sk_number_clone(sk_vm_get_proto(vm, "Number"));
    sk_object_put_slot(sk_vm_get_proto(vm, "Object"), "foo", foo);
 
    SkNode *node = sk_bytecode_parse_filename("test.sk");
    SkObject *number = sk_ast_evaluate(vm, node);
    printf("Number: %d\n", sk_number_get_int(number));
//    printf("String: %s\n", bstr2cstr(sk_string_get_bstring(number), ' '));

    return 0;
}
