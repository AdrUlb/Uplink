#pragma once

#include <cstdio>

bool RsFileEncryptedNoVerify(const char* path);
bool RsFileEncrypted(const char* path);
FILE* RsFileOpen(const char* path, const char* mode);
void RsFileClose(const char* path, FILE* file);
const char* RsArchiveFileOpen(const char* path);
void RsArchiveFileClose(const char* path, FILE* file);
bool RsEncryptFile(const char* path);
