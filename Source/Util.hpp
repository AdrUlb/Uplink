#pragma once

#include <cstring>
#include <print>
#include <source_location>
#include <unistd.h>
#include <sys/stat.h>

#include "UplinkObject.hpp"
#include "_.hpp"
#include "Tosser/BTree.hpp"

bool FileReadDataInt(std::source_location loc, void* ptr, size_t size, size_t count, FILE* file);
void DeleteBTreeData(BTree<char*>& btree);
bool LoadBTree(BTree<char*>& btree, FILE* file);
void SaveBTree(BTree<char*>& btree, FILE* file);
void PrintBTree(BTree<char*>& btree);

#define FileReadData(ptr, size, count, file) FileReadDataInt(std::source_location::current(), ptr, size, count, file)

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
        if (const auto result__ = snprintf(buffer, max, format, ##args); result__ < 0 || result__ >= max) \
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

template<typename T> requires std::is_base_of_v<UplinkObject, T>
void DeleteBTreeData(BTree<T*>& btree)
{
	const auto* const darray = btree.ConvertToDArray();

	for (size_t i = 0; i < darray->Size(); i++)
	{
		if (!darray->ValidIndex(i))
			continue;

		delete darray->GetData(i);
	}

	delete darray;
}

template<typename T> requires std::is_base_of_v<UplinkObject, T>
bool LoadBTree(BTree<T*>& btree, FILE* file)
{
	TODO_ABORT;
}

template<typename T> requires std::is_base_of_v<UplinkObject, T>
void SaveBTree(BTree<T*>& btree, FILE* file)
{
	TODO_ABORT;
}

template<typename T> requires std::is_base_of_v<UplinkObject, T>
void PrintBTree(BTree<T*>& btree)
{
	const auto* const darray = btree.ConvertToDArray();
	const auto* const indices = btree.ConvertIndexToDArray();

	for (size_t i = 0; i < darray->Size(); i++)
	{
		if (!darray->ValidIndex(i))
			continue;

		UplinkAssert(indices->ValidIndex(i));

		std::println("Index = {}", indices->GetData(i));

		const auto data = darray->GetData(i);
		if (!data)
		{
			std::println("NULL");
			continue;
		}

		data->Print();
	}

	delete indices;
	delete darray;
}
