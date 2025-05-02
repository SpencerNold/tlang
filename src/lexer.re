#include <stdlib.h>
#include <string.h>

#include "lexer.h"

/*!max:re2c*/
#define SIZE 64 * 1024

struct lex_token_stream_t {
    tok_queue_t* queue;
    int value;
};

struct lex_input_t {
    FILE* file;
    unsigned char buf[SIZE + YYMAXFILL];
    unsigned char* limit;
    unsigned char* current;
    unsigned char* marker;
    unsigned char* token;
    int eof;
};

lex_input_t* lex_open(FILE* file) {
    lex_input_t* input = (lex_input_t*) malloc(sizeof(lex_input_t));
    if (input == NULL) {
        return NULL;
    }
    input->file = file;
    input->limit = input->buf + SIZE;
    input->current = input->limit;
    input->marker = input->limit;
    input->token = input->limit;
    input->eof = 0;
    return input;
}

void lex_close_input(lex_input_t* input) {
    free(input);
}

int fill(lex_input_t* input, size_t size) {
    if (input->eof) {
        return 0;
    }
    const size_t free = input->token - input->buf;
    if (free < size) {
        return 0;
    }
    memmove(input->buf, input->token, input->limit - input->token);
    input->limit -= free;
    input->current -= free;
    input->marker -= free;
    input->token -= free;
    input->limit += fread(input->limit, 1, free, input->file);
    if (input->limit < input->buf + SIZE) {
        input->eof = 1;
        memset(input->limit, 0, YYMAXFILL);
        input->limit += YYMAXFILL;
    }
    return 1;
}

void _append_bool_literal(lex_token_stream_t* stream, int type) {
    append_int_tok_queue(stream->queue, type, type == TRUE_VALUE);
}

lex_token_stream_t* lex_run(lex_input_t* input) {
    lex_token_stream_t* stream = (lex_token_stream_t*) malloc(sizeof(lex_token_stream_t));
    stream->queue = allocate_tok_queue();
    stream->value = 0;
    for (;;) {
        input->token = input->current;
        /*!re2c
            re2c:yyfill:enable = 1;
            re2c:define:YYCTYPE = char;
            re2c:define:YYCURSOR = input->current;
            re2c:define:YYMARKER = input->marker;
            re2c:define:YYLIMIT = input->limit;
            re2c:define:YYFILL = "if (!fill(input, @@)) return 0;";
            re2c:define:YYFILL:naked = 1;
            
            end = "\x00";
            end {
                stream->value = input->limit - input->token == YYMAXFILL;
                break;
            }

            comment = "//" [^\n]* "\n";
            whitespace = ([ \t\v\n\r] | comment)+;
            whitespace { continue; }

            // String & Numeric Literals

            // identifier = [a-zA-Z_][a-zA-Z_0-9]*;
            // identifier { printf("%.*s\n", (int)(input->current - input->token), input->token); continue; }

            "true" {
                _append_bool_literal(stream, TRUE_VALUE);
            }
            "false" {
                _append_bool_literal(stream, FALSE_VALUE);
            }

            * { 
                lex_close_token_stream(stream);
                return NULL;
            }
        */
    }
    return stream;
}

token_t* lex_stream_next(lex_token_stream_t* stream) {
    return poll_tok_queue(stream->queue);
}

int lex_token_type(token_t* token) {
    
}

void lex_free_token(token_t* token) {
    free_token(token);
}

void lex_close_token_stream(lex_token_stream_t* stream) {
    free_tok_queue(stream->queue);
    free(stream);
}