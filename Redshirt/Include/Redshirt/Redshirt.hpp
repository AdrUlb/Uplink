#pragma once

#include <cstdint>

#include "Bungle/Bungle.hpp"
#include "Tosser/DArray.hpp"

bool RsFileExists(const char* path);
bool RsMakeDirectory(char const* path);
void RsDeleteDirectory(const char* path);
const char* RsBasename(const char* path);
void RsFileCheckSum(FILE* file, uint8_t* buffer, size_t size);
bool RsFileEncryptedNoVerify(const char* path);
bool RsFileEncrypted(const char* path);
FILE* RsFileOpen(const char* path, const char* mode);
void RsFileClose(const char* path, FILE* file);
bool RsEncryptFile(const char* path);
bool RsDecryptFile(const char* path);
void RsInitialise(const char* appPath);
void RsCleanUp();
bool RsLoadArchive(const char* id);
void RsCloseArchive(const char* id);
bool RsArchiveFileLoaded(const char* filename);
DArray<char*>* RsListArchive(const char* dir, const char* ext);
const char* RsArchiveFileOpen(const char* filename);
FILE* RsArchiveFileOpen(const char* filename, const char* mode);
void RsArchiveFileClose(const char* filename, FILE* file);
