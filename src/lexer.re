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

void _append_floating_point_literal(lex_token_stream_t* stream, int type, double value) {
    append_float_tok_queue(stream->queue, type, value);
}

void _append_decimal_literal(lex_token_stream_t* stream, int type, long long value) {
    append_int_tok_queue(stream->queue, type, value);
}

void _append_bool_literal(lex_token_stream_t* stream, int type) {
    append_int_tok_queue(stream->queue, type, type == TRUE_VALUE);
}

void _append_str_literal(lex_token_stream_t* stream, int type, char* str, int len) {
    append_str_tok_queue(stream->queue, type, str, len);
}

void _append_token(lex_token_stream_t* stream, int type) {
    append_tok_queue(stream->queue, type);
}

double _parse_float_from_re2c(lex_input_t* input) {
    const int len = (int) (input->current - input->token);
    char buf[len + 1];
    strncpy(buf, (char*) input->token, len);
    buf[len] = 0;
    return atof(buf);
}

long long _parse_dec_from_re2c(lex_input_t* input) {
    const int len = (int) (input->current - input->token);
    char buf[len + 1];
    strncpy(buf, (char*) input->token, len);
    buf[len] = 0;
    return atoll(buf);
}

long long _parse_hex_from_re2c(lex_input_t* input) {
    const int len = (int) (input->current - input->token);
    char buf[len + 1];
    strncpy(buf, (char*) input->token, len);
    buf[len] = 0;
    return strtoll(buf, NULL, 16);
}

char* _parse_str_from_re2c(lex_input_t* input) {
    unsigned char quote = input->current[-1];
    unsigned char* start = input->token;
    int length;
    for (length = 1;;) {
        length++;
        /*!re2c
            re2c:yyfill:enable = 0;
            re2c:define:YYCTYPE = char;
            re2c:define:YYCURSOR = start;
            * { 
                unsigned char c = start[0];
                if (c == quote) break;
                continue;
            }
        */
    }

    char buf[length];
    int index = 0;
    buf[index++] = quote;
    for (unsigned char c = quote;;) {
        input->token = input->current;
        /*!re2c
            re2c:yyfill:enable = 1;
            re2c:define:YYCTYPE = char;
            re2c:define:YYCURSOR = input->current;
            re2c:define:YYMARKER = input->marker;
            re2c:define:YYLIMIT = input->limit;
            re2c:define:YYFILL = "if (!fill(input, @@)) { return NULL; }";
            re2c:define:YYFILL:naked = 1;
            
            [^\n\\] {
                c = input->token[0];
                buf[index++] = c;
                if (c == quote) break;
                continue;
            }
            "\\r" { buf[index++] = '\r'; continue; }
            "\\n" { buf[index++] = '\n'; continue; }
            "\\t" { buf[index++] = '\t'; continue; }
            "\\f" { buf[index++] = '\f'; continue; }
            "\\b" { buf[index++] = '\b'; continue; }
            "\\\\" { buf[index++] = '\\'; continue; }
            "\\" ["'`] {
                if (input->token[1] != quote) {
                    buf[index++] = input->token[0];
                }
                buf[index++] = input->token[1];
                continue;
            }
            * { return NULL; }
        */
    }
    buf[index] = 0;
    return (char*) strdup(buf);
}

lex_token_stream_t* lex_run(lex_input_t* input) {
    lex_token_stream_t* stream = (lex_token_stream_t*) malloc(sizeof(lex_token_stream_t));
    if (stream == NULL) {
        return NULL;
    }
    stream->queue = allocate_tok_queue();
    stream->value = ERROR;
    for (;;) {
        input->token = input->current;
        /*!re2c
            re2c:yyfill:enable = 1;
            re2c:define:YYCTYPE = char;
            re2c:define:YYCURSOR = input->current;
            re2c:define:YYMARKER = input->marker;
            re2c:define:YYLIMIT = input->limit;
            re2c:define:YYFILL = "if (!fill(input, @@)) { stream->value = 0; return stream; }";
            re2c:define:YYFILL:naked = 1;
            
            end = "\x00";
            end {
                stream->value = input->limit - input->token == YYMAXFILL;
                break;
            }

            comment = "//" [^\n]* "\n";
            whitespace = ([ \t\v\n\r] | comment)+;
            whitespace { continue; }

            // String Literal
            ["'`] {
                char* str = _parse_str_from_re2c(input);
                if (str == NULL) {
                    stream->value = ERROR;
                    return stream;
                }
                int len = strlen(str);
                _append_str_literal(stream, STRING_LIT, str, len);
                free(str);
                continue;
            }

            // Numeric Literal
            [1-9][0-9]* { // Decimal
                long long value = _parse_dec_from_re2c(input);
                _append_decimal_literal(stream, INTEGER, value);
                continue;
            }
            '0x' [0-9a-fA-F]+ { // Hex
                long long value = _parse_hex_from_re2c(input);
                _append_decimal_literal(stream, INTEGER, value);
                continue;
            }
            [0-9]* "." [0-9]+ | [0-9]+ "." { // Float
                double value = _parse_float_from_re2c(input);
                _append_floating_point_literal(stream, FLOATING_POINT, value);
                continue;
            }

            // PL Tokens
            "(" { _append_token(stream, O_PARENTHESES); continue; }
            ")" { _append_token(stream, C_PARENTHESES); continue; }
            "{" { _append_token(stream, O_C_BRACKET); continue; }
            "}" { _append_token(stream, C_C_BRACKET); continue; }
            "[" { _append_token(stream, O_BRACKET); continue; }
            "]" { _append_token(stream, C_BRACKET); continue; }
            "." { _append_token(stream, DOT); continue; }
            "," { _append_token(stream, COMMA); continue; }

            // Keywords
            "include" { _append_token(stream, INCLUDE); continue; }
            "private" { _append_token(stream, PRIVATE); continue; }
            "protected" { _append_token(stream, PROTECTED); continue; }
            "class" { _append_token(stream, CLASS); continue; }
            "enum" { _append_token(stream, ENUM); continue; }
            "interface" { _append_token(stream, INTERFACE); continue; }
            "new" { _append_token(stream, NEW); continue; }
            "extends" { _append_token(stream, EXTENDS); continue; }
            "implements" { _append_token(stream, IMPLEMENTS); continue; }
            "abstract" { _append_token(stream, ABSTRACT); continue; }
            "final" { _append_token(stream, FINAL); continue; }
            "assert" { _append_token(stream, ASSERT); continue; }
            "this" { _append_token(stream, THIS); continue; }
            "return" { _append_token(stream, RETURN); continue; }
            "if" { _append_token(stream, IF); continue; }
            "else" { _append_token(stream, ELSE); continue; }
            "for" { _append_token(stream, FOR); continue; }
            "in" { _append_token(stream, IN); continue; }
            "range" { _append_token(stream, RANGE); continue; }
            "while" { _append_token(stream, WHILE); continue; }
            "continue" { _append_token(stream, CONTINUE); continue; }
            "break" { _append_token(stream, BREAK); continue; }
            "is" { _append_token(stream, IS); continue; }
            "async" { _append_token(stream, ASYNC); continue; }
            "future" { _append_token(stream, FUTURE); continue; }
            "unsafe" { _append_token(stream, UNSAFE); continue; }
            "defer" { _append_token(stream, DEFER); continue; }

            // Types
            "int8" { _append_token(stream, INT8); continue; }
            "int16" { _append_token(stream, INT16); continue; }
            "int32" { _append_token(stream, INT32); continue; }
            "int64" { _append_token(stream, INT64); continue; }
            "uint8" { _append_token(stream, UINT8); continue; }
            "uint16" { _append_token(stream, UINT16); continue; }
            "uint32" { _append_token(stream, UINT32); continue; }
            "uint64" { _append_token(stream, UINT64); continue; }
            "float32" { _append_token(stream, FLOAT32); continue; }
            "float64" { _append_token(stream, FLOAT64); continue; }
            "char" { _append_token(stream, CHAR); continue; }
            "bool" { _append_token(stream, BOOL); continue; }
            "function" { _append_token(stream, FUNCTION); continue; }
            "void" { _append_token(stream, VOID); continue; }
            "string" { _append_token(stream, STRING); continue; }

            "true" { _append_bool_literal(stream, TRUE_VALUE); continue; }
            "false" { _append_bool_literal(stream, FALSE_VALUE); continue; }
            "null" { _append_bool_literal(stream, NULL_VALUE); continue; }

            // Operators
            "=" { _append_token(stream, EQ_OPER); continue; }
            "+" { _append_token(stream, ADD_OPER); continue; }
            "-" { _append_token(stream, SUB_OPER); continue; }
            "*" { _append_token(stream, MUL_OPER); continue; }
            "/" { _append_token(stream, DIV_OPER); continue; }
            "%" { _append_token(stream, MOD_OPER); continue; }
            "++" { _append_token(stream, INCR_OPER); continue; }
            "--" { _append_token(stream, DECR_OPER); continue; }
            "+=" { _append_token(stream, ADD_EQ_OPER); continue; }
            "-=" { _append_token(stream, SUB_EQ_OPER); continue; }
            "*=" { _append_token(stream, MUL_EQ_OPER); continue; }
            "/=" { _append_token(stream, DIV_EQ_OPER); continue; }
            "%=" { _append_token(stream, MOD_EQ_OPER); continue; }
            "**" { _append_token(stream, POW_OPER); continue; }
            ">" { _append_token(stream, GT_OPER); continue; }
            "<" { _append_token(stream, LT_OPER); continue; }
            ">=" { _append_token(stream, GT_EQ_OPER); continue; }
            "<=" { _append_token(stream, LT_EQ_OPER); continue; }
            ("&&" | "and") { _append_token(stream, AND_OPER); continue; }
            ("||" | "or") { _append_token(stream, OR_OPER); continue; }
            "!" { _append_token(stream, NOT_OPER); continue; }
            "?" { _append_token(stream, TERNARY_OPER); continue; }
            ("&" | "bitand") { _append_token(stream, BIT_AND_OPER); continue; }
            ("|" | "bitor") { _append_token(stream, BIT_OR_OPER); continue; }
            ("^" | "xor") { _append_token(stream, BIT_XOR_OPER); continue; }
            ("~" | "compl") { _append_token(stream, BIT_NOT_OPER); continue; }
            (">>" | "shr") { _append_token(stream, BIT_SHR_OPER); continue; }
            ("<<" | "shl") { _append_token(stream, BIT_SHL_OPER); continue; }

            identifier = [a-zA-Z_][a-zA-Z_0-9]*;
            identifier {
                int len = (int) (input->current - input->token);
                _append_str_literal(stream, IDENTIFIER, (char*) input->token, len);
                continue;
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

void lex_free_token(token_t* token) {
    free_token(token);
}

void lex_close_token_stream(lex_token_stream_t* stream) {
    free_tok_queue(stream->queue);
    free(stream);
}