#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits.h>
#include <print>
#include <unistd.h>
#include <sys/stat.h>

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
	({ if (!(condition)) std::println("Print Abort: " __FILE__ " ln " XSTRINGIFY(__LINE__) " : " format __VA_OPT__(,) __VA_ARGS__); !(condition); })
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
				" Dest. size  : {}\n" \
				" Source size : {}\n" \
				" Str. Source : {}\n", \
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
		const auto result = snprintf(buffer, max, format __VA_OPT__(,) __VA_ARGS__); \
		if (max < 0 || result < 0 || static_cast<size_t>(result) >= static_cast<size_t>(max)) \
		{ \
			std::print( \
				"\n" \
				"An Uplink snprintf Failure has occured\n" \
				"======================================\n" \
				" Location    : {}, line {}\n" \
				" Buffer size : {}\n" \
				" Format      : {}\n" \
				" Buffer      : {}\n", \
				__FILE__, __LINE__, max, format, buffer \
			); \
			abort(); \
		} \
	} \
	while (0)

#define UplinkAbort(message) \
	do \
	{ \
		printf( \
			"\n" \
			"Uplink has been forced to Abort\n" \
			"===============================\n" \
			" Message   : %s\n" \
			" Location  : %s, line %d\n", \
			message, __FILE__, __LINE__ \
		); \
		abort(); \
	} \
	while (0)

#define LoadData(ptr, size, file) \
	({ \
		const auto success = FileReadDataInt(__FILE__, __LINE__, ptr, size, 1, file); \
		success; \
	})

#define SaveData(ptr, size, file) ({ fwrite(ptr, size, 1, file) == 1; })

#define LoadFixedString(buffer, size, file) \
	({ \
		const auto success = FileReadDataInt(__FILE__, __LINE__, buffer, size, 1, file); \
		if (success) \
			buffer[size - 1] = '\0'; \
		else \
			buffer[0] = '\0'; \
		success; \
	})

#define SaveFixedString(buffer, size, file) ({ fwrite(buffer, size, 1, file) == 1; })

#define LoadDynamicString(buffer, file) LoadDynamicStringInt(__FILE__, __LINE__, buffer, file)

bool FileReadDataInt(const char* sourceFile, int sourceLine, void* ptr, size_t size, size_t count, FILE* file);

// TODO: make consistent
bool LoadDynamicStringInt(const char* sourceFile, int sourceLine, char*& buffer, FILE* file);
void SaveDynamicString(const char* buffer, int maxSize, FILE* file);

static void SaveDynamicString(const char* buffer, FILE* file)
{
	SaveDynamicString(buffer, -1, file);
}

static bool DoesFileExist(const char* path)
{
	return access(path, 0) == 0;
}

static bool MakeDirectory(const char* path)
{
	return mkdir(path, 0700);
}

class UplinkObject;

bool LoadBTree(BTree<UplinkObject*>* tree, FILE* file);

template<std::derived_from<UplinkObject> T> bool LoadBTree(BTree<T*>* tree, FILE* file)
{
	return LoadBTree(reinterpret_cast<BTree<UplinkObject*>*>(tree), file);
}

void SaveBTree(BTree<UplinkObject*>* tree, FILE* file);

template<std::derived_from<UplinkObject> T> void SaveBTree(BTree<T*>* tree, FILE* file)
{
	SaveBTree(reinterpret_cast<BTree<UplinkObject*>*>(tree), file);
}

void DeleteBTreeData(BTree<UplinkObject*>* tree);

template<std::derived_from<UplinkObject> T> void DeleteBTreeData(BTree<T*>* tree)
{
	DeleteBTreeData(reinterpret_cast<BTree<UplinkObject*>*>(tree));
}

void PrintBTree(BTree<UplinkObject*>* tree);

template<std::derived_from<UplinkObject> T> void PrintBTree(BTree<T*>* tree)
{
	PrintBTree(reinterpret_cast<BTree<UplinkObject*>*>(tree));
}
