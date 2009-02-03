#ifndef BYTECODE_H_INCLUDED
#define BYTECODE_H_INCLUDED

SkNode *sk_bytecode_parse_filename(const char *filename);
SkNode *sk_bytecode_parse_stream(FILE *stream);
SkNode *sk_bytecode_parse_block(FILE *stream);
SkNode *sk_bytecode_parse_number(FILE *stream);
SkNode *sk_bytecode_parse_node(FILE *stream);
SkNode *sk_bytecode_parse_nil(FILE *stream);

#endif
