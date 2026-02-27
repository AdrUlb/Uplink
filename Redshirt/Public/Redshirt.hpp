#pragma once

#include <cstdint>

#include "Tosser/DArray.hpp"

void RsInitialise(const char* appPath);

void RsFileCheckSum(FILE* file, uint8_t* buffer);
bool RsFileEncrypted(const char* path);
bool RsFileEncryptedNoVerify(const char* path);

FILE* RsFileOpen(const char* path, const char* modes);
void RsFileClose(const char* path, FILE* file);

bool RsLoadArchive(const char* id);
void RsCloseArchive(const char* id);

bool RsArchiveFileLoaded(const char* name);
std::optional<std::string> RsArchiveFileOpen(const char* name);
FILE* RsArchiveFileOpen(const char* name, const char* mode);
void RsArchiveFileClose(std::string_view path, FILE* file);

DArray<const char*>* RsListArchive(const char* dir, const char* query);
bool RsDecryptFile(const char* path);
bool RsEncryptFile(const char* path);
