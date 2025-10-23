#include "Bungle/Bungle.hpp"

#include <sys/stat.h>

#include "_.hpp"
#include "Tosser/LList.hpp"

static BTree<LocalFileHeader*> files;

/**
 * Create a directory.
 * @param path Where to create the directory at.
 * @return True if the directory was created, false otherwise.
 */
static bool BglMakeDirectory(const char* path)
{
	return mkdir(path, 0700) == 0;
}

/**
 * Normalize slashes and replace all uppercase letters with lowercase in a path.
 * @param path The path to modify.
 */
static void BglSlashify(std::string& path)
{
	for (auto* str = strchr(path.data(), '\\'); str; str = strchr(str, '\\'))
		*str = '/';

	for (auto* str = path.data(); *str != 0; str++)
	{
		if (*str >= 'A' && *str <= 'Z')
			*str += 'a' - 'A';
	}
}

/**
 * Load a ZIP archive, preparing its files for extraction.
 * @param archiveFile The FILE to read from.
 * @param apppath The apppath.
 * @param id The archive ID.
 * @return True if the archive was loaded successfully, false otherwise.
 */
bool BglOpenZipFile(FILE* archiveFile, const char* apppath, const char* id)
{
	if (!archiveFile)
		return false;

	while (!feof(archiveFile))
	{
		auto* fileHeader = new LocalFileHeader();
		fread(&fileHeader->signature, sizeof(fileHeader->signature), 1, archiveFile);

		if (((fileHeader->signature >> 0) & 0xFF) != 'P' || ((fileHeader->signature >> 8) & 0xFF) != 'K')
		{
			delete fileHeader;
			break;
		}

		fread(&fileHeader->version, sizeof(fileHeader->version), 1, archiveFile);
		fread(&fileHeader->flags, sizeof(fileHeader->flags), 1, archiveFile);
		fread(&fileHeader->compression, sizeof(fileHeader->compression), 1, archiveFile);
		fread(&fileHeader->modTime, sizeof(fileHeader->modTime), 1, archiveFile);
		fread(&fileHeader->modDate, sizeof(fileHeader->modDate), 1, archiveFile);
		fread(&fileHeader->crc32, sizeof(fileHeader->crc32), 1, archiveFile);
		fread(&fileHeader->compressedSize, sizeof(fileHeader->compressedSize), 1, archiveFile);
		fread(&fileHeader->uncompressedSize, sizeof(fileHeader->uncompressedSize), 1, archiveFile);
		fread(&fileHeader->filenameLen, sizeof(fileHeader->filenameLen), 1, archiveFile);
		fread(&fileHeader->extraFieldLen, sizeof(fileHeader->extraFieldLen), 1, archiveFile);

		if (fileHeader->compression != 0 || fileHeader->compressedSize != fileHeader->uncompressedSize)
		{
			delete fileHeader;
			break;
		}

		if (fileHeader->filenameLen > 0)
		{
			fileHeader->filename.resize(fileHeader->filenameLen);
			fread(fileHeader->filename.data(), fileHeader->filenameLen, 1, archiveFile);
		}

		std::string extraField;
		if (fileHeader->extraFieldLen > 0)
		{
			extraField.resize(fileHeader->extraFieldLen);
			fread(extraField.data(), fileHeader->extraFieldLen, 1, archiveFile);
		}

		if (fileHeader->uncompressedSize > 0)
		{
			fileHeader->data = new char[fileHeader->uncompressedSize + 1];
			fread(fileHeader->data, fileHeader->uncompressedSize, 1, archiveFile);
			fileHeader->data[fileHeader->uncompressedSize] = 0;
		}

		if (!fileHeader->data || fileHeader->filename.empty())
		{
			delete[] fileHeader->data;
			delete fileHeader;
			continue;
		}

		if (id)
			fileHeader->id = id;

		auto filename = std::format("{}{}", apppath, fileHeader->filename);
		BglSlashify(filename);
		files.PutData(filename.c_str(), fileHeader);
	}

	return true;
}

/**
 * Load a ZIP archive, preparing its files for extraction.
 * @param archivePath The path of the file to read from.
 * @param apppath The apppath.
 * @param id The archive ID.
 * @return True if the archive was loaded successfully, false otherwise.
 */
bool BglOpenZipFile(const char* archivePath, const char* apppath, const char* id)
{
	auto* file = fopen(archivePath, "rb");
	if (!file)
		return false;

	const auto success = BglOpenZipFile(file, apppath, id);
	fclose(file);
	return success;
}

static void BglCloseZipFile_Recursive(BTree<LocalFileHeader*>* files, LList<const char*>* removableIds, const char* id)
{
	assert(removableIds);
	assert(id);

	for (const auto* i = files; i; i = i->Right())
	{
		const auto* fileHeader = i->GetData();

		if (fileHeader)
		{
			const auto fileId = fileHeader->id;

			if (fileId && *fileId == id)
				removableIds->PutData(i->Id());
		}

		BglCloseZipFile_Recursive(i->Left(), removableIds, id);
	}
}

/**
 * Unprepare all files from a ZIP archive.
 * @param id The archive ID.
 */
void BglCloseZipFile(const char* id)
{
	LList<const char*> removableIds;
	BglCloseZipFile_Recursive(&files, &removableIds, id);

	for (size_t i = 0; i < removableIds.Size(); i++)
	{
		auto* filename = removableIds.GetData(i);
		assert(filename);

		const auto* fileHeader = files.LookupData(filename);
		assert(fileHeader);

		files.RemoveData(filename);

		delete[] fileHeader->data;
		delete fileHeader;
	}
}

/**
 * Extract all files from a ZIP archive.
 * @param archivePath The path of the file to read from.
 */
void BglExtractAllFiles(const char* archivePath)
{
	FILE* archiveFile = fopen(archivePath, "rb");

	assert(archiveFile);

	while (!feof(archiveFile))
	{
		LocalFileHeader fileHeader;
		fread(&fileHeader.signature, sizeof(fileHeader.signature), 1, archiveFile);

		if (((fileHeader.signature >> 0) & 0xFF) != 'P' || ((fileHeader.signature >> 8) & 0xFF) != 'K')
			break;

		fread(&fileHeader.version, sizeof(fileHeader.version), 1, archiveFile);
		fread(&fileHeader.flags, sizeof(fileHeader.flags), 1, archiveFile);
		fread(&fileHeader.compression, sizeof(fileHeader.compression), 1, archiveFile);
		fread(&fileHeader.modTime, sizeof(fileHeader.modTime), 1, archiveFile);
		fread(&fileHeader.modDate, sizeof(fileHeader.modDate), 1, archiveFile);
		fread(&fileHeader.crc32, sizeof(fileHeader.crc32), 1, archiveFile);
		fread(&fileHeader.compressedSize, sizeof(fileHeader.compressedSize), 1, archiveFile);
		fread(&fileHeader.uncompressedSize, sizeof(fileHeader.uncompressedSize), 1, archiveFile);
		fread(&fileHeader.filenameLen, sizeof(fileHeader.filenameLen), 1, archiveFile);
		fread(&fileHeader.extraFieldLen, sizeof(fileHeader.extraFieldLen), 1, archiveFile);

		if (fileHeader.compression != 0 || fileHeader.compressedSize != fileHeader.uncompressedSize)
			break;

		bool isDirectory = false;
		if (fileHeader.filenameLen > 0)
		{
			fileHeader.filename.resize(fileHeader.filenameLen);
			fread(fileHeader.filename.data(), fileHeader.filenameLen, 1, archiveFile);
			isDirectory = fileHeader.filename[fileHeader.filenameLen - 1] == '/';
		}

		std::string extraField;
		if (fileHeader.extraFieldLen > 0)
		{
			extraField.resize(fileHeader.extraFieldLen);
			fread(fileHeader.filename.data(), fileHeader.extraFieldLen, 1, archiveFile);
		}

		if (isDirectory)
		{
			BglMakeDirectory(fileHeader.filename.c_str());
		}
		else if (fileHeader.uncompressedSize > 0 && !fileHeader.filename.empty())
		{
			fileHeader.data = new char[fileHeader.uncompressedSize + 1];
			fread(fileHeader.data, fileHeader.uncompressedSize, 1, archiveFile);
			fileHeader.data[fileHeader.uncompressedSize] = 0;

			FILE* file = fopen(fileHeader.filename.c_str(), "wb");
			assert(file);
			fwrite(fileHeader.data, fileHeader.uncompressedSize, 1, file);
			fclose(file);
		}

		delete[] fileHeader.data;
	}

	fclose(archiveFile);
}

/**
 * Determine whether a file is loaded and ready for extraction.
 * @param name The name of the file.
 * @return True if the file is ready for extraction, false otherwise.
 */
bool BglFileLoaded(const char* name)
{
	auto slashifiedName = std::string(name);
	BglSlashify(slashifiedName);
	const auto* header = files.LookupData(slashifiedName.c_str());

	return header;
}

/**
 * Extract a file.
 * @param name The name of the file.
 * @param extractPath The extraction destination.
 * @return Whether the operation completed successfully.
 */
bool BglExtractFile(const char* name, const char* extractPath)
{
	auto slashifiedName = std::string(name);
	BglSlashify(slashifiedName);
	const auto* header = files.LookupData(slashifiedName.c_str());

	if (!header)
		return false;

	const auto file = fopen(extractPath ? extractPath : name, "wb");

	if (!file)
		return false;

	fwrite(header->data, header->uncompressedSize, 1, file);
	fclose(file);
	return true;
}

/**
 * Close all given files, unpreparing them for extraction.
 * @param files The list of files to close.
 */
void BglCloseAllFiles(BTree<LocalFileHeader*>* files)
{
	if (!files)
		return;

	BglCloseAllFiles(files->Left());
	BglCloseAllFiles(files->Right());
	const auto* fileHeader = files->GetData();

	if (fileHeader)
	{
		delete[] fileHeader->data;
		delete fileHeader;
	}

	files->Empty();
}

void BglCloseAllFiles()
{
	BglCloseAllFiles(&files);
}

/**
 * List all files matching the given query parameters.
 * @param apppath The apppath.
 * @param dir The archive directory to search in.
 * @param query The string to search for.
 * @return List of files which matched the pattern.
 */
DArray<const char*>* BglListFiles(char const* apppath, char const* dir, char const* query)
{
	auto queryDir = std::format("{}{}", apppath, dir);
	BglSlashify(queryDir);

	auto* filenames = files.ConvertIndexToDArray();

	for (size_t i = 0; i < filenames->Size(); i++)
	{
		if (!filenames->ValidIndex(i))
			continue;

		const auto* filename = filenames->GetData(i);
		auto fileDir = std::string(filename).substr(0, queryDir.size());

		if (strcmp(fileDir.c_str(), queryDir.c_str()) != 0 || !strstr(filename, query))
			filenames->RemoveData(i);
	}

	return filenames;
}
