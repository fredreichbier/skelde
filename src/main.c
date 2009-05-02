#include <stdio.h>

#include "vm.h"
#include "skstring.h"
#include "bstrlib.h"
#include "object.h"
#include "number.h"
#include "message.h"
#include "bytecode.h"

SkObject *sk_test(SkObject *slot, SkObject *self, SkObject *msg) {
    printf("TEST CALLED\n");
    return self->vm->nil;
}

int main(int argc, char** argv) {
    SkVM *vm = sk_vm_new();
    sk_object_bind_method(vm->lobby, "test", &sk_test);
    sk_object_set_slot(vm->lobby, "foo", sk_string_from_bstring(vm, bfromcstr("bar")));

    SkObject *avalanche = sk_bytecode_parse_filename(vm, "test.sk");
    SkObject *result = sk_message_dispatch_avalanche(avalanche);

    printf("should be 'bar': %s\n", bstr2cstr(sk_string_get_bstring(result), '\\'));

    return 0;
}
