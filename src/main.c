#include <stdio.h>

#include "vm.h"

#include "object.h"
#include "number.h"
#include "list.h"

int main(int argc, char** argv) {
    SkVM *vm = sk_vm_new();
    sk_vm_add_proto(vm, "Object", sk_object_create_proto(vm));
    sk_vm_add_proto(vm, "Number", sk_number_create_proto(vm));
    sk_vm_add_proto(vm, "List", sk_list_create_proto(vm));
    
    SkObject *number = sk_object_clone(
            sk_object_get_slot_lazy(
                    vm->lobby,
                    "Number")
            );


    SkObject *list = sk_object_clone(
            sk_object_get_slot_lazy(
                vm->lobby,
                "List")
            );
    sk_list_append(list, number);

    SkObject *number2 = sk_list_get_at(list, 0);
    printf("Number: %d\n", sk_number_get_int(number2));

    return 0;
}
