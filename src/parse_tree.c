#include "parser/parse_tree.h"

#include "stdio.h"
#include <stdlib.h>

node_t* create_node_value(int type, void* value) {
    node_t* node = (node_t*) malloc(sizeof(node_t));
    if (node == NULL) return NULL;
    node->type = type;
    node->value = value;
    node->children = NULL;
    node->n = 0;
    return node;
}

node_t* create_node(int type) {
    return create_node_value(type, NULL);
}

int add_node(node_t* parent, node_t* child) {
    parent->children = realloc(parent->children, sizeof(node_t*) * (parent->n + 1));
    if (parent->children == NULL) return 1;
    parent->children[parent->n++] = child;
    return 0;
}

void free_node(node_t* node) {
    if (node == NULL) return;
    for (int i = 0; i < node->n; i++) {
        free_node(node->children[i]);
    }
    if (node->value != NULL) {
        free(node->value);
    }
    free(node->children);
    free(node);
}