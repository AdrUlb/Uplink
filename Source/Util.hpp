#pragma once

#include <cstdio>

#define UplinkSnprintf(buffer, max, format, args...) \
    do \
    { \
        const auto result = snprintf(buffer, max, format, ##args); \
        if (result < 0 || result >= max) \
        { \
            printf( \
                "\n" \
                "An Uplink snprintf Failure has occured\n" \
                "======================================\n" \
                " Location    : %s, line %d\n Buffer size : \n" \
                " Format      : %s\n Buffer      : %s\n", \
                __FILE__, __LINE__, max, format, buffer); \
            *(char*)0 = 0; \
        } \
    } \
    while (false)
