#ifndef PARSE_TREE_H
#define PARSE_TREE_H

typedef struct _node_t {
    int type;

    struct _node_t** children;
    int n;
} node_t;

node_t* create_node(int type);
int add_node(node_t* parent, node_t* child);
void free_node(node_t* node);

#endif