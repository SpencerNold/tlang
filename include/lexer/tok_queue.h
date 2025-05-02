#ifndef TOK_QUEUE_H
#define TOK_QUEUE_H

typedef struct tok_queue_t tok_queue_t;
typedef struct token_t token_t;

tok_queue_t* allocate_tok_queue();
token_t* poll_tok_queue(tok_queue_t* queue);
token_t* peek_tok_queue(tok_queue_t* queue);
int get_tok_type(token_t* token);
long long get_int_token_value(token_t* token);
double get_float_tok_value(token_t* token);
int append_tok_queue(tok_queue_t* queue, int type);
int append_int_tok_queue(tok_queue_t* queue, int type, long long i);
int append_float_tok_queue(tok_queue_t* queue, int type, double fp);
int append_str_tok_queue(tok_queue_t* queue, int type, const char* str, int len);
void free_token(token_t* token);
void free_tok_queue(tok_queue_t* queue);

#endif