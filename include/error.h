#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>

#define ERRORF(format, ...) { printf((format), ##__VA_ARGS__); exit(1); }

#endif