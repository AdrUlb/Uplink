#include "Bungle/Bungle.hpp"

#include "_.hpp"

void BglSlashify(char* path)
{
	for (auto* str = strchr(path, '\\'); str; str = strchr(str, '\\'))
		*str = '/';

	for (auto* str = path; *str != 0; str++)
	{
		if (*str >= 'A' && *str <= 'Z')
			*str += 'a' - 'A';
	}
}

bool BglOpenZipFile(FILE* archiveFile, const char* apppath, const char* id)
{
	TODO_ABORT;
}

bool BglOpenZipFile(const char* archivePath, const char* apppath, const char* id)
{
	auto* file = fopen(archivePath, "rb");
	if (!file)
		return false;

	const auto success = BglOpenZipFile(file, apppath, id);
	fclose(file);
	return success;
}

void BglCloseZipFile(const char* id)
{
	TODO_ABORT;
}

void BglExtractAllFiles(const char* archivePath)
{
	TODO_ABORT;
}

bool BglFileLoaded(const char* name)
{
	TODO_ABORT;
}

bool BglExtractFile(const char* name, const char* extractPath)
{
	TODO_ABORT;
}

void BglCloseAllFiles(BTree<LocalFileHeader*>* files)
{
	TODO_ABORT;
}

void BglCloseAllFiles()
{
	TODO_ABORT;
}

DArray<char*>* BglListFiles(char const* apppath, char const* dir, char const* query)
{
	TODO_ABORT;
}
