#include <stdio.h>
#include <assert.h>

#include "version.h"
#include "mem.h"
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
    assert(fread(sig, sizeof(char), 4, stream) == 4);
    if(!(sig[0] == 's' && sig[1] == 'k' 
                && sig[2] == SK_BYTECODE_VERSION_MAJOR 
                && sig[3] == SK_BYTECODE_VERSION_MINOR)) {
        printf("The bytecode signature is not supported.\n");
        return NULL;
    }
    /* now, parse */
    return NULL;//sk_bytecode_parse_avalanche(vm, stream);
    /* left over stuff is left over */
}


