#pragma once

#include <cstdint>

#include "Tosser/BTree.hpp"
#include "Tosser/DArray.hpp"

struct LocalFileHeader
{
	uint32_t signature = 0;
	uint16_t version = 0;
	uint16_t flags = 0;
	uint16_t compression = 0;
	uint16_t modTime = 0;
	uint16_t modDate = 0;
	uint32_t crc32 = 0;
	uint32_t compressedSize = 0;
	uint32_t uncompressedSize = 0;
	uint16_t filenameLen = 0;
	uint16_t extraFieldLen = 0;
	std::string filename;
	std::optional<std::string> id = { };
	char* data = nullptr;
};

bool BglOpenZipFile(FILE* archiveFile, const char* apppath, const char* id);
bool BglOpenZipFile(const char* archivePath, const char* apppath, const char* id);
void BglCloseZipFile(const char* id);
void BglExtractAllFiles(const char* archivePath);
bool BglFileLoaded(const char* name);
bool BglExtractFile(const char* name, const char* extractedPath);
void BglCloseAllFiles(BTree<LocalFileHeader*>* files);
void BglCloseAllFiles();
DArray<const char*>* BglListFiles(char const* apppath, char const* dir, char const* query);
