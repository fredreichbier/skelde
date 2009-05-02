#ifndef BYTECODE_H_INCLUDED
#define BYTECODE_H_INCLUDED

#include "vm.h"

typedef enum _SkOpcode {
    SK_OPCODE_NONE,
    SK_OPCODE_MESSAGE,
    SK_OPCODE_EOA
} SkOpcode;

SkObject *sk_bytecode_parse_filename(SkVM *vm, const char *filename);
SkObject *sk_bytecode_parse_stream(SkVM *vm, FILE *stream);
SkObject *sk_bytecode_parse_avalanche(SkVM *vm, FILE *stream);

#endif
