#pragma once

#include "Tosser/DArray.hpp"

void BglCloseAllFiles();
void BglCloseZipFile(const char* id);
DArray<const char*>* BglListFiles(const char* appPath, const char* dirName, const char* query);
bool BglExtractFile(const char* fileName, const char* destPath);
bool BglFileLoaded(const char* fileName);
bool BglOpenZipFile(FILE* file, const char* appPath, const char* id);
bool BglOpenZipFile(const char* filePath, const char* appPath, const char* id);
