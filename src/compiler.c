#include "compiler.h"

#include "lexer.h"
#include "parser.h"
#include "compiler/errors.h"

#include <string.h>

void print(int level, node_t* node) {
    for (int i = 0; i < level; i++) printf("\t");
    printf("%04X", node->type);
    printf("\n");
}

void walk(node_t* node, int level, void (*func)(int, node_t*)) {
    printf("%d\n", node->n);
    func(level, node);
    for (int i = 0; i < node->n; i++) {
        node_t* child = node->children[i];
        if (child->n != 0) {
            walk(child, level + 1, func);
        } else {
            func(level + 1, child);
        }
    }
}

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

    node_t* tree = parse_file(stream);


    free_node(tree);
    lex_close_input(input);
    lex_close_token_stream(stream);
    context->errno = 0;
}