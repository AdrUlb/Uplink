#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>

void RsFileCheckSum(FILE* file, uint8_t* buffer, size_t size);
bool RsFileEncryptedNoVerify(const char* path);
bool RsFileEncrypted(const char* path);
FILE* RsFileOpen(const char* path, const char* mode);
void RsFileClose(const char* path, FILE* file);
const char* RsArchiveFileOpen(const char* path);
void RsArchiveFileClose(const char* path, FILE* file);
bool RsEncryptFile(const char* path);
