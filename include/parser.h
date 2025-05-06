#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

#include "parser/parse_types.h"
#include "parser/parse_tree.h"

node_t* parse_file(lex_token_stream_t* stream);

#endif