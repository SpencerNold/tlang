#ifndef COMPILER_H
#define COMPILER_H

typedef struct _context_t {
    int errno;
} context_t;

void compile_file(context_t* context, char* path);

#endif