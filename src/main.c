#include "lexer.h"

int main() {
    FILE* file = fopen("test.tl", "r");
    if (file == NULL) {
        fprintf(stderr, "failed to open source file test.tl\n");
        return 1;
    }
    lex_input_t* input = lex_open(file);
    lex_token_stream_t* stream = lex_run(input);

    token_t* token = lex_stream_next(stream);
    printf("token: %lld\n", get_int_token_value(token));
    free_token(token);

    token = lex_stream_next(stream);
    printf("token: %lld\n", get_int_token_value(token));
    free_token(token);

    lex_close_input(input);
    lex_close_token_stream(stream);
}