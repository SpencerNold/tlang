#include "compiler.h"

int main() {
    context_t context;
    compile_file(&context, "stdlib/test.tl");
}