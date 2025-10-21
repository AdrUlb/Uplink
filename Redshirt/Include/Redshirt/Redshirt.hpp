#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>

bool RsFileExists(const char* path);
void RsDeleteDirectory(const char* path);
const char* RsBasename(const char* path);
void RsFileCheckSum(FILE* file, uint8_t* buffer, size_t size);
bool RsFileEncryptedNoVerify(const char* path);
bool RsFileEncrypted(const char* path);
FILE* RsFileOpen(const char* path, const char* mode);
void RsFileClose(const char* path, FILE* file);
bool RsEncryptFile(const char* path);
bool RsDecryptFile(const char* path);
const char* RsArchiveFileOpen(const char* name);
FILE* RsArchiveFileOpen(const char* name, const char* mode);
void RsArchiveFileClose(const char* name, FILE* file);
void RsInitialise(const char* appPath);
void RsCleanUp();
