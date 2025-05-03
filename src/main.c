#include "compiler.h"

#include <stdlib.h>
#include <stdio.h>

int main() {
    context_t context;
    compile_file(&context, "stdlib/test.tl");
}