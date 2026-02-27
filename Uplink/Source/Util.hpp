#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <print>
#include <unistd.h>

#include "Tosser/BTree.hpp"

#define XSTRINGIFY(x) STRINGIFY(x)
#define STRINGIFY(x) #x

#define PrintAssert(condition) \
	if (!(condition)) \
	{ \
		std::println("Print Assert: " __FILE__ " ln " XSTRINGIFY(__LINE__) " : " #condition); \
		return false; \
	}

#define PrintAbort(condition, format, ...) \
	if (!(condition)) \
	{ \
		std::println("Print Abort: " __FILE__ " ln " XSTRINGIFY(__LINE__) " : " format __VA_OPT__(,) __VA_ARGS__); \
		return false; \
	}

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

#define UplinkSnprintf(buffer, max, format, ...) \
	do \
	{ \
		if (snprintf(buffer, max, format __VA_OPT__(,) __VA_ARGS__) >= max) \
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

#define LoadFixedString(buffer, size, file) \
	({ \
		const auto success = FileReadDataInt(__FILE__, __LINE__, buffer, size, 1, file); \
		if (success) \
			buffer[size - 1] = '\0'; \
		else \
			buffer[0] = '\0'; \
		success; \
	})

#define SaveFixedString(buffer, file) ({ fwrite(buffer, sizeof(buffer), 1, file) == 1; })

#define LoadData(ptr, size, file) \
	({ \
		const auto success = FileReadDataInt(__FILE__, __LINE__, ptr, size, 1, file); \
		success; \
	})

#define WriteData(ptr, size, file) ({ fwrite(ptr, size, 1, file) == 1; })

#define LoadDynamicString(buffer, file) LoadDynamicStringInt(__FILE__, __LINE__, buffer, file)

bool FileReadDataInt(const char* sourceFile, int sourceLine, void* ptr, size_t size, size_t count, FILE* file);

bool LoadDynamicStringInt(const char* sourceFile, const int sourceLine, char*& buffer, FILE* file);

static bool DoesFileExist(const char* name)
{
	return access(name, 0) == 0;
}

class UplinkObject;

bool LoadBTree(BTree<UplinkObject*>* tree, FILE* file);

template<std::derived_from<UplinkObject> T> bool LoadBTree(BTree<T*>* tree, FILE* file)
{
	return LoadBTree(reinterpret_cast<BTree<UplinkObject*>*>(tree), file);
}

void DeleteBTreeData(BTree<UplinkObject*>* tree);

template<std::derived_from<UplinkObject> T> void DeleteBTreeData(BTree<T*>* tree)
{
	DeleteBTreeData(reinterpret_cast<BTree<UplinkObject*>*>(tree));
}
