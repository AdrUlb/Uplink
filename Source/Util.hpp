#pragma once

#include <cstring>
#include <print>
#include <source_location>

bool FileReadDataInt(std::source_location loc, void* ptr, size_t size, size_t count, FILE* file);

#define FileReadData(ptr, size, count, file) FileReadDataInt(std::source_location::current(), ptr, size, count, file)

#define UplinkAssert(condition) \
	do \
	{ \
		if (!(condition)) \
		{ \
			std::print( \
				"\n" \
				"An Uplink Assertion Failure has occured\n" \
				"=======================================\n" \
				" Condition : {}\n" \
				" Location  : {}, line {}\n", \
				#condition, __FILE__, __LINE__); \
			*(volatile char*)0 = 0; \
		} \
	} \
	while (false)

#define UplinkSnprintf(buffer, max, format, args...) \
    do \
    { \
        const auto result = snprintf(buffer, max, format, ##args); \
        if (result < 0 || result >= max) \
        { \
            std::print( \
                "\n" \
                "An Uplink snprintf Failure has occured\n" \
                "======================================\n" \
                " Location    : {}, line {}\n Buffer size : {}\n" \
                " Format      : {}\n Buffer : {}\n", \
                __FILE__, __LINE__, max, format, buffer); \
            *(volatile char*)0 = 0; \
        } \
    } \
    while (false)

#define UplinkStrncpy(dest, src, max) \
	do \
	{ \
		if (strlen(src) >= max) \
		{ \
			std::print( \
				"\n" \
				"An Uplink strncpy Failure has occured\n" \
				"=====================================\n" \
				" Location    : {}, line {}\n" \
				" Dest. size  : {}\n" \
				" Source size : {}\n" \
				" Str. Source : {}\n", \
				__FILE__, __LINE__, max, strlen(src), src); \
			*(volatile char*)0 = 0; \
		}\
		strncpy(dest, src, max); \
		dest[(max) - 1] = 0; \
	} \
	while (false)
