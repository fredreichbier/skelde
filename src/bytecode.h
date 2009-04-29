#ifndef BYTECODE_H_INCLUDED
#define BYTECODE_H_INCLUDED

#include "vm.h"

SkObject *sk_bytecode_parse_filename(SkVM *vm, const char *filename);
SkObject *sk_bytecode_parse_stream(SkVM *vm, FILE *stream);

#endif
