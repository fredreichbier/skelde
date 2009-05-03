#include <assert.h>

#include "message.h"
#include "objlist.h"
#include "skstring.h"
#include "number.h"
#include "list.h"

/* a message has the following slots:
   * name
   * arguments
 */

/* init: clone `Message`. don't set name, arguments, next, previous from proto ... */
void sk_message_init(SkObject *self) {
    sk_object_set_clone_func(self, &sk_message_clone);
    sk_object_set_init_func(self, &sk_message_init2);
    sk_message_set_name(self,
            sk_string_from_bstring(self->vm, bfromcstr("Aargh")));
    sk_message_set_arguments(self,
            sk_list_create(self->vm));
    sk_message_set_next(self,
            self->vm->nil);
    sk_message_set_previous(self,
            self->vm->nil);
}

/* init2: clone a `Message` clone */
void sk_message_init2(SkObject *self) {
    sk_object_set_clone_func(self, &sk_message_clone);
    sk_object_set_init_func(self, &sk_message_init2);
/*    SkObject *proto = sk_object_get_proto(self);
    sk_message_set_name(self,
            sk_message_get_name(proto));
    sk_message_set_arguments(self,
            sk_message_get_arguments(proto));
    sk_message_set_next(self,
            sk_message_get_next(proto));
    sk_message_set_previous(self,
            sk_message_get_previous(proto));*/
}

SkObject *sk_message_create_proto(SkVM* vm) {
    SkObject *self = sk_object_clone(sk_vm_get_proto(vm, "Object"));
    sk_object_set_init_func(self, &sk_message_init);
    sk_object_set_clone_func(self, &sk_message_clone);
    
    /* methods */
    sk_object_bind_method(self, "dispatch", &sk_message__dispatch);
    return self;
}

DEFINE_LAZY_CLONE_FUNC(sk_message_clone);

static _Bool is_number(bstring string) {
    int i;
    char ch = bchar(string, 0);
    /* if the first char is neither a digit nor a '+' nor a '-', it's invalid. */
    if(!(isdigit(ch) || ch == '+' || ch == '-')) {
        return 0;
    }
    /* if the first char is not a digit (-> '+' or '-'), but the string is only
     * one char long, it's invalid.*/
    if(!isdigit(ch) && string->slen == 1) {
        return 0;
    }
    /* check all following, have to be digits. */
    for(i = 1; i < string->slen; i++) {
        ch = bchar(string, i);
        if(!isdigit(ch)) {
            return 0;
        }
    }
    return 1;
}

SkObject *sk_message_dispatch_simple(SkObject *self) {
    SkObject *result = NULL;
    bstring name = sk_string_get_bstring(sk_message_get_name(self));
    /* is a string */
    if(bchar(name, 0) == '"' && bchar(name, name->slen - 1) == '"') {
        return sk_string_from_bstring(self->vm, bmidstr(name, 1, name->slen - 2));
    } 
    /* is a number */
    else if(is_number(name)) {
        return sk_number_create(self->vm, atoi(bstr2cstr(name, '\0')));
    }
    /* is a command terminator. */
    else if(biseqcstr(name, ";") == 1) {
        return NULL;
    }
    /* a message. */
    else {
        int i;
        for(i = cvector_size(self->vm->callstack) - 1; i >= 0; i--) {
            SkObject *object = objlist_get_at(self->vm->callstack, i);
            result = sk_object_dispatch_message(object, self);
            if(result) {
                return result;
            }
        }
        printf("The message '%s' couldn't be dispatched. I'm sorry.\n", bstr2cstr(name, '\\'));
        return NULL;
    }
}

SkObject *sk_message_start_dispatch(SkObject *self) {
    assert(sk_message_get_previous(self) == self->vm->nil);
    /* duplicate the top element */
    sk_vm_callstack_push(self->vm, sk_vm_callstack_top(self->vm));
    SkObject *msg = self, *result;
    do {
        result = sk_message_dispatch_simple(msg);
        if(!result) {
            /* command terminator (or unhandled message, but that's TODO),
             * pop and push the topmost element again */
            sk_vm_callstack_pop(self->vm);
            sk_vm_callstack_push(self->vm, sk_vm_callstack_top(self->vm));
        } else {
            /* set topmost element to the new result. */
            sk_vm_callstack_pop(self->vm);
            sk_vm_callstack_push(self->vm, result);
        }
        msg = sk_message_get_next(msg);
    } while(msg != self->vm->nil);
    sk_vm_callstack_pop(self->vm);
    return result;
}

SkObject *sk_message_dispatch_avalanche(SkObject *self) {
    while(sk_message_get_previous(self) != self->vm->nil)
        self = sk_message_get_previous(self);
    return sk_message_start_dispatch(self);
}

SkObject *sk_message_create_simple(SkVM *vm, char *name) {
    SkObject *self = sk_object_clone(sk_vm_get_proto(vm, "Message"));
    sk_message_set_name(self,
            sk_string_from_bstring(vm, bfromcstr(name)));
    return self;
}

SkObject *sk_message__dispatch(SkObject *slot, SkObject *self, SkObject *msg) {
    return sk_message_dispatch_avalanche(self);
}
