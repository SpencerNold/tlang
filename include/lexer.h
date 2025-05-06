#ifndef LEXER_H
#define LEXER_H

#include "lexer/tok_queue.h"
#include "lexer/tokens.h"

#include <stdio.h>

typedef struct lex_input_t lex_input_t;
typedef struct lex_token_stream_t lex_token_stream_t;

lex_input_t* lex_open(FILE* file);
lex_token_stream_t* lex_run(lex_input_t* input);
token_t* lex_stream_next(lex_token_stream_t* stream);
token_t* lex_stream_check(lex_token_stream_t* stream);

void lex_close_token_stream(lex_token_stream_t* stream);
void lex_free_token(token_t* token);
void lex_close_input(lex_input_t* input);

#endif