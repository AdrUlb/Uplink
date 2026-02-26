#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <print>
#include <unistd.h>

#define UplinkAssert(condition) \
	do \
	{ \
		if (!(condition)) \
		{ \
			std::print( \
				"\n" \
				"An Uplink Assertion Failure has occured\n" \
				"======================================\n" \
				" Condition   : {}\n" \
				" Location    : {}, line {}\n", \
				#condition, __FILE__, __LINE__ \
			); \
			abort(); \
		} \
	} \
	while (0)

#define UplinkStrncpy(dest, source, max) \
	do \
	{ \
		const auto sourceLen = strlen(source); \
		if (sourceLen >= max) \
		{ \
			std::print( \
				"\n" \
				"An Uplink strncpy Failure has occured\n" \
				"======================================\n" \
				" Location    : {}, line {}\n" \
				" Dest. size  : %d\n" \
				" Source size : %s\n" \
				" Str. Source : %s\n", \
				__FILE__, __LINE__, max, sourceLen, source \
			); \
			abort(); \
		} \
		strncpy(dest, source, max); \
		dest[max - 1] = '\0'; \
	} \
	while (0)

#define UplinkSnprintf(buffer, max, format, args...) \
	do \
	{ \
		if (snprintf(buffer, max, format, args) >= max) \
		{ \
			std::print( \
				"\n" \
				"An Uplink snprintf Failure has occured\n" \
				"======================================\n" \
				" Location    : {}, line {}\n" \
				" Buffer size : %d\n" \
				" Format      : %s\n" \
				" Buffer      : %s\n", \
				__FILE__, __LINE__, max, format, buffer \
			); \
			abort(); \
		} \
	} \
	while (0)

#define Load_StringBuf(buffer, file) \
	do \
	{ \
		if (FileReadDataInt(__FILE__, __LINE__, buffer, sizeof(buffer), 1, file)) \
		{ \
			_name[sizeof(buffer) - 1] = '\0'; \
		} \
		else \
		{ \
			_name[0] = '\0'; \
			return false; \
		} \
	} \
	while (0)

#define Save_StringBuf(buffer, file) \
	do \
	{ \
		fwrite(buffer, sizeof(buffer), 1, file); \
	} \
	while (0)

#define Load_Field(value, file) \
	do \
	{ \
		if (!FileReadDataInt(__FILE__, __LINE__, &value, sizeof(value), 1, file)) \
			return false; \
	} \
	while (0);

#define Save_Field(value, file) \
	do \
	{ \
		fwrite(&value, sizeof(value), 1, file); \
	} \
	while (0);

bool FileReadDataInt(const char* sourceFile, int sourceLine, void* ptr, size_t size, size_t count, FILE* file);

static bool DoesFileExist(const char* name)
{
	return access(name, 0) == 0;
}
