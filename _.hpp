#pragma once

#include <cstdio>
#include <cstdlib>

#define TODO_ABORT \
    do { \
        fprintf(stderr, "ABORT: TODO: %s:%d in %s\n", __FILE__, __LINE__, __func__); \
        abort(); \
    } while (0)
#define TODO_PRINT \
	static bool todoPrinted = false; \
    do { \
		if (!todoPrinted) \
		{ \
			fprintf(stderr, "TODO: %s:%d in %s\n", __FILE__, __LINE__, __func__); \
			todoPrinted = true; \
		} \
    } while (0)
