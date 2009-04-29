#include <stdio.h>

#include "vm.h"
#include "skstring.h"
#include "object.h"
#include "number.h"
#include "bytecode.h"

SkObject *sk_string_print(SkObject *self, SkObject *message) {
    printf("%s\n", sk_string_as_charp(self));
    return self;
}

int main(int argc, char** argv) {
    SkVM *vm = sk_vm_new();
    sk_object_set_method(sk_vm_get_proto(vm, "String"), "print", &sk_string_print);

    sk_bytecode_parse_filename(vm, "test.sk");
    SkObject *slot = sk_object_get_slot_lazy(vm->lobby, "foo");
    printf("should be 'bar': %s\n", bstr2cstr(sk_string_get_bstring(slot), ' '));

    return 0;
}
