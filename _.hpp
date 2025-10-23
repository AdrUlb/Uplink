#pragma once

#include <cstdio>
#include <cstdlib>

#define TODO_ABORT \
    do { \
        fprintf(stderr, "ABORT: TODO: %s:%d in %s\n", __FILE__, __LINE__, __func__); \
        abort(); \
    } while (0)
