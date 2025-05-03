#include "lexer/tok_queue.h"

#include <stdlib.h>
#include <string.h>

struct token_t {
    int type;
    void* value;

    token_t* next;
};

struct tok_queue_t {
    token_t* start;
    token_t* end;
};

int _append_tok_queue(tok_queue_t*, int, void*);

// gives ownership of tok_queue_t*
tok_queue_t* allocate_tok_queue() {
    tok_queue_t* queue = (tok_queue_t*) malloc(sizeof(tok_queue_t));
    if (queue == NULL) {
        return NULL;
    }
    queue->start = NULL;
    queue->end = NULL;
    return queue;
}

// gives ownership of token_t*
token_t* poll_tok_queue(tok_queue_t* queue) {
    if (queue->start == NULL) {
        return NULL;
    }
    token_t* token = queue->start;
    queue->start = token->next;
    token->next = NULL;
    return token;
}

int get_tok_type(token_t* token) {
    return token->type;
}

char* get_str_tok_value(token_t* token) {
    return (char*) token->value;
}

long long get_int_token_value(token_t* token) {
    return *((long long*) token->value);
}

double get_float_tok_value(token_t* token) {
    return *((double*) token->value);
}

token_t* peek_tok_queue(tok_queue_t* queue) {
    return queue->start == NULL ? NULL : queue->start;
}

int append_tok_queue(tok_queue_t* queue, int type) {
    return _append_tok_queue(queue, type, NULL);
}

int append_int_tok_queue(tok_queue_t* queue, int type, long long i) {
    long long* value = (long long*) malloc(sizeof(long long));
    if (value == NULL) {
        return 1;
    }
    *value = i;
    return _append_tok_queue(queue, type, value);
}

int append_float_tok_queue(tok_queue_t* queue, int type, double fp) {
    double* value = (double*) malloc(sizeof(double));
    if (value == NULL) {
        return 1;
    }
    *value = fp;
    return _append_tok_queue(queue, type, value);
}

int append_str_tok_queue(tok_queue_t* queue, int type, char* str, int len) {
    char* value = (char*) malloc(sizeof(len + 1));
    if (value == NULL) {
        return 1;
    }
    strncpy(value, str, len);
    value[len] = 0;
    return _append_tok_queue(queue, type, value);
}

int _append_tok_queue(tok_queue_t* queue, int type, void* value) {
    token_t* token = (token_t*) malloc(sizeof(token_t));
    if (token == NULL) {
        return 1;
    }
    token->type = type;
    token->value = value;
    token->next = NULL;
    if (queue->start == NULL) {
        queue->start = token;
        queue->end = token;
    } else {
        queue->end->next = token;
        queue->end = token;
    }
    return 0;
}

void free_token(token_t* token) {
    void* value = token->value;
    if (value != NULL) {
        free(value);
    }
    free(token);
}

void free_tok_queue(tok_queue_t* queue) {
    token_t* token = queue->start;
    while (token != NULL) {
        token_t* next = token->next;
        free_token(token);
        token = next;
    }
    free(queue);
}