#include "parser.h"

#include "error.h"

node_t* parse_include(lex_token_stream_t*);
node_t* parse_declaration(lex_token_stream_t*);
node_t* parse_class(lex_token_stream_t*);

node_t* parse_file(lex_token_stream_t* stream) {
    node_t* root = create_node(NT_FILE);
    if (root == NULL) ERRORF("COMPILER ERROR: Out of memory!\n");
    token_t* token = lex_stream_check(stream);
    if (token == NULL) ERRORF("COMPILER ERROR: Out of memory!\n");
    int type = get_tok_type(token);
    if (type == INCLUDE) { // tok element of First(include), file -> include
        node_t* include = parse_include(stream);
        add_node(root, include);
    } else if (IS_TYPE(TYPE_MASK, type)) { // tok element of First(file), file -> declaration
        node_t* declaration = parse_declaration(stream);
        add_node(root, declaration);
    } else if (type == CLASS) {
        node_t* class = parse_class(stream);
        add_node(root, class);
    } else if (type == NOP) { // tok element of Follow(file), file -> lambda (Follow(file) == $)
    }
    return root;
}

node_t* parse_include(lex_token_stream_t* stream) {
    node_t* root = create_node(NT_INCLUDE);
    if (root == NULL) ERRORF("COMPILER ERROR: Out of memory!\n");
    token_t* token = lex_stream_check(stream);
    if (token == NULL) ERRORF("COMPILER ERROR: Out of memory!\n");
    int type = get_tok_type(token);
    if (type == INCLUDE) {
        // include
        token = lex_stream_next(stream);
        node_t* include = create_node(INCLUDE);
        if (include == NULL) ERRORF("COMPILER ERROR: Out of memory!\n");
        add_node(root, include);
        free(token);
        // *STRING_LIT*
        token = lex_stream_next(stream);
        if (token == NULL) ERRORF("COMPILER ERROR: Out of memory!\n");
        if (get_tok_type(token) != STRING_LIT) ERRORF("SYNTAX ERROR: include ^^^\n");
        node_t* target = create_node_value(STRING_LIT, get_str_tok_value(token));
        if (target == NULL) ERRORF("COMPILER ERROR: Out of memory!\n");
        add_node(root, target);
        free(token);
    } else {
        ERRORF("SYMANTIC ERROR\n");
    }
    return root;
}

node_t* parse_declaration(lex_token_stream_t* stream) {
    return NULL;
}

node_t* parse_class(lex_token_stream_t* stream) {
    return NULL;
}