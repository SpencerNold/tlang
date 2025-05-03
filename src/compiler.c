#include "compiler.h"

#include "lexer.h"
#include "compiler/errors.h"

#include <string.h>

void compile_file(context_t* context, char* path) {
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        context->errno = ERR_FILE_NOT_FOUND;
        return;
    }

    // Lexing
    lex_input_t* input = lex_open(file);
    if (input == NULL) {
        context->errno = ERR_INTERNAL;
        fclose(file);
        return;
    }
    lex_token_stream_t* stream = lex_run(input);
    if (stream == NULL) {
        context->errno = ERR_INTERNAL;
        fclose(file);
        lex_close_input(input);
        return;
    }

    token_t* token = lex_stream_next(stream);
    printf("type: 0x%04x\n", get_tok_type(token));
    char* str = get_str_tok_value(token);
    printf("value: %s\n", str);
    printf("len: %lu\n", strlen(str));
    free_token(token);

    lex_close_input(input);
    lex_close_token_stream(stream);
    context->errno = 0;
}