#pragma once

#include <cstring>
#include <dirent.h>
#include <print>
#include <source_location>
#include <unistd.h>
#include <sys/stat.h>
#include "Tosser/BTree.hpp"

#include "UplinkObject.hpp"

bool FileReadDataInt(std::source_location loc, void* ptr, size_t size, size_t count, FILE* file);
bool LoadDynamicStringInt(std::source_location loc, char*& ptr, FILE* file);
bool LoadDynamicStringInt(std::source_location loc, char* buffer, size_t max, FILE* file);
void SaveDynamicString(const char* str, FILE* file);
void SaveDynamicString(const char* str, size_t max, FILE* file);

bool LoadBTree(BTree<UplinkObject*>& btree, FILE* file);
void SaveBTree(const BTree<UplinkObject*>& btree, FILE* file);
void PrintBTree(const BTree<UplinkObject*>& btree);
void DeleteBTreeData(const BTree<UplinkObject*>& btree);

bool LoadBTree(BTree<char*>& btree, FILE* file);
void SaveBTree(const BTree<char*>& btree, FILE* file);
void PrintBTree(const BTree<char*>& btree);
void DeleteBTreeData(const BTree<char*>& btree);

void EmptyDirectory(const char* path);
std::string GetFilePath(const std::string_view path);

// ReSharper disable CppRedundantInlineSpecifier
template<typename T> requires (std::is_base_of_v<UplinkObject, T> && !std::is_same_v<UplinkObject, T>)
__attribute__((always_inline)) inline bool LoadBTree(BTree<T*>& btree, FILE* file)
{
	return LoadBTree(*reinterpret_cast<BTree<UplinkObject*>*>(&btree), file);
}

template<typename T> requires (std::is_base_of_v<UplinkObject, T> && !std::is_same_v<UplinkObject, T>)
__attribute__((always_inline)) inline void SaveBTree(BTree<T*>& btree, FILE* file)
{
	SaveBTree(*reinterpret_cast<BTree<UplinkObject*>*>(&btree), file);
}

template<typename T> requires (std::is_base_of_v<UplinkObject, T> && !std::is_same_v<UplinkObject, T>)
__attribute__((always_inline)) inline void PrintBTree(BTree<T*>& btree)
{
	PrintBTree(*reinterpret_cast<BTree<UplinkObject*>*>(&btree));
}

template<typename T> requires (std::is_base_of_v<UplinkObject, T> && !std::is_same_v<UplinkObject, T>)
__attribute__((always_inline)) inline void DeleteBTreeData(BTree<T*>& btree)
{
	DeleteBTreeData(*reinterpret_cast<BTree<UplinkObject*>*>(&btree));
}

// ReSharper restore CppRedundantInlineSpecifier

#define FileReadData(ptr, size, count, file) FileReadDataInt(std::source_location::current(), ptr, size, count, file)
#define LoadDynamicString(ptr, file) LoadDynamicStringInt(std::source_location::current(), ptr, file)
#define LoadDynamicStringBuffer(buffer, max, file) LoadDynamicStringInt(std::source_location::current(), buffer, max, file)

#define UplinkAbort(message) \
	do \
	{ \
		std::print( \
			"\n" \
			"Uplink has been forced to Abort\n" \
			"===============================\n" \
			" Message  : {}\n" \
			" Location : {}, line {}\n", \
			message, __FILE__, __LINE__); \
		*(volatile char*)0 = 0; \
		__builtin_unreachable(); \
	} \
	while (false)

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
			__builtin_unreachable(); \
		} \
	} \
	while (false)

#define UplinkSnprintf(buffer, max, format, args...) \
    do \
    { \
        if (const auto result__ = snprintf(buffer, max, format, ##args); result__ < 0 || static_cast<size_t>(result__) >= max) \
        { \
            std::print( \
                "\n" \
                "An Uplink snprintf Failure has occured\n" \
                "======================================\n" \
                " Location    : {}, line {}\n Buffer size : {}\n" \
                " Format      : {}\n Buffer : {}\n", \
                __FILE__, __LINE__, max, format, buffer); \
            *(volatile char*)0 = 0; \
			__builtin_unreachable(); \
        } \
        buffer[max - 1] = 0; \
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
			__builtin_unreachable(); \
		}\
		strncpy(dest, src, max); \
		dest[(max) - 1] = 0; \
	} \
	while (false)

inline bool DoesFileExist(const char* path)
{
	return access(path, F_OK) == 0;
}

inline void MakeDirectory(const char* path)
{
	mkdir(path, 0700);
}
