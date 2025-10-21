#pragma once

#include <cstdint>

#include "Tosser/BTree.hpp"
#include "Tosser/DArray.hpp"

struct LocalFileHeader
{
	uint32_t signature;
	uint16_t version;
	uint16_t flags;
	uint16_t compression;
	uint16_t modTime;
	uint32_t modDate;
	uint32_t crc32;
	uint32_t compressedSize;
	uint32_t uncompressedSize;
	uint16_t fileNameLen;
	uint16_t extraFieldLen;
	char* filename;
	char* extraField;
	char* filedata;
	char* id;
};

void BglSlashify(char* path);
bool BglOpenZipFile(FILE* archiveFile, const char* apppath, const char* id);
bool BglOpenZipFile(const char* archivePath, const char* apppath, const char* id);
void BglCloseZipFile(const char* id);
void BglExtractAllFiles(const char* archivePath);
bool BglFileLoaded(const char* name);
bool BglExtractFile(const char* name, const char* extractedPath);
void BglCloseAllFiles(BTree<LocalFileHeader*>* files);
void BglCloseAllFiles();
DArray<char*>* BglListFiles(char const* apppath, char const* dir, char const* query);