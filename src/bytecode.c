#include <stdio.h>
#include <assert.h>

#include "version.h"
#include "mem.h"
#include "list.h"
#include "message.h"
#include "string.h"
#include "vm.h"
#include "bytecode.h"

SkObject *sk_bytecode_parse_filename(SkVM *vm, const char *filename) {
    FILE *file = fopen(filename, "rb");
    assert(file != NULL);
    SkObject *root = sk_bytecode_parse_stream(vm, file);
    fclose(file);
    return root;
}

SkObject *sk_bytecode_parse_stream(SkVM *vm, FILE *stream) {
    /* check signature. the first four bytes of the file
     * have to be 'sk' + the major bytecode version + the
     * minor bytecode version (as ordinal values)
     */
    char sig[4];
    size_t bytes_read;
    if((bytes_read = fread(sig, sizeof(char), 4, stream)) != 4) {
        if(stream == stdin) {
            // expected end of input.
            exit(0);
        } else {
            // non-stdin input? errorz!
            sk_printf("Oh I can't read enough bytes. I wanted 4, got %d.\n", bytes_read);
            abort();
        }
    }
    if(!(sig[0] == 's' && sig[1] == 'k'
                && sig[2] == SK_BYTECODE_VERSION_MAJOR 
                && sig[3] == SK_BYTECODE_VERSION_MINOR)) {
        sk_printf("The given bytecode signature is not supported.\n");
        abort();
    }
    /* now, parse */
    return sk_bytecode_parse_avalanche(vm, stream);
    /* left over stuff is left over */
}

static int _fgetc(void *STREAM) {
    return fgetc(STREAM);
}

SkObject *sk_bytecode_parse_message(SkVM *vm, FILE *stream) {
    SkObject *self = sk_object_clone(sk_vm_get_proto(vm, "Message"));
    bstring name = bgets(&_fgetc, stream, '\0');
    /* cut the trailing \0 */
    bdelete(name, name->slen - 1, 1);
    /* set the message's name and read the argument count */
    sk_message_set_name(self, sk_string_from_bstring(vm, name));
    ArgCount argcount, i;
    if(fread(&argcount, sizeof(ArgCount), 1, stream) != 1) {
        abort();
    }
    for(i = 0; i < argcount; i++) {
        SkObject *arg = sk_bytecode_parse_avalanche(vm, stream);
        sk_message_append_argument(self, arg); 
    }
    return self;
}

SkObject *sk_bytecode_parse_avalanche(SkVM *vm, FILE *stream) {
    SkObject *previous = vm->nil, *msg = NULL;
    for(;;) {
        unsigned char opcode;
        if(fread(&opcode, sizeof(unsigned char), 1, stream) != 1) {
            abort();
        }
        switch(opcode) {
            case SK_OPCODE_MESSAGE:
                msg = sk_bytecode_parse_message(vm, stream);
                sk_message_set_previous(msg, previous);
                if(previous != vm->nil) {
                    sk_message_set_next(previous, msg);
                }
                previous = msg;
                break;
            case SK_OPCODE_EOA:
                return msg;
            default:
                sk_printf("Unknown opcode: %u\n", opcode);
                abort();
        }
    }
}

