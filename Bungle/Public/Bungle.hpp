#pragma once

#include "Tosser/DArray.hpp"

bool BglOpenZipFile(FILE* file, const char* appPath, const char* id);
bool BglOpenZipFile(const char* filePath, const char* appPath, const char* id);
void BglCloseZipFile(const char* id);
void BglCloseAllFiles();

bool BglFileLoaded(const char* fileName);
bool BglExtractFile(const char* fileName, const char* destPath);
DArray<const char*>* BglListFiles(const char* appPath, const char* dir, const char* query);
