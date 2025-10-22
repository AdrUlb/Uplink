#include "Bungle/Bungle.hpp"

#include <sys/stat.h>

#include "_.hpp"
#include "Tosser/LList.hpp"

static BTree<LocalFileHeader*> files;

static bool BglMakeDirectory(const char* path)
{
	return mkdir(path, 0700) == 0;
}

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
			fileHeader->filename = new char[fileHeader->filenameLen + 1];
			fread(fileHeader->filename, fileHeader->filenameLen, 1, archiveFile);
			fileHeader->filename[fileHeader->filenameLen] = 0;
		}
		else
			fileHeader->filename = nullptr;

		if (fileHeader->extraFieldLen > 0)
		{
			fileHeader->extraField = new char[fileHeader->extraFieldLen + 1];
			fread(fileHeader->extraField, fileHeader->extraFieldLen, 1, archiveFile);
			fileHeader->extraField[fileHeader->extraFieldLen] = 0;
		}
		else
			fileHeader->extraField = nullptr;

		if (fileHeader->uncompressedSize > 0)
		{
			fileHeader->data = new char[fileHeader->uncompressedSize + 1];
			fread(fileHeader->data, fileHeader->uncompressedSize, 1, archiveFile);
			fileHeader->data[fileHeader->uncompressedSize] = 0;
		}
		else
			fileHeader->data = nullptr;

		if (!fileHeader->data || !fileHeader->filename)
		{
			delete[] fileHeader->filename;
			delete[] fileHeader->extraField;
			delete[] fileHeader->data;
			delete fileHeader;
			continue;
		}

		if (id)
		{
			fileHeader->id = new char[strlen(id) + 1];
			strcpy(fileHeader->id, id);
		}
		else
			fileHeader->id = nullptr;

		auto filename = std::format("{}{}", apppath, fileHeader->filename);
		BglSlashify(filename);
		files.PutData(filename.c_str(), fileHeader);
	}

	return true;
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

static void BglCloseZipFile_Recursive(BTree<LocalFileHeader*>* files, LList<const char*>* removableIds, const char* id)
{
	assert(removableIds);
	assert(id);

	for (const auto* i = files; i; i = i->Right())
	{
		const auto* data_ = i->GetData();

		if (data_)
		{
			const auto* fileId = data_->id;

			if (fileId && !strcmp(fileId, id))
				removableIds->PutData(i->Id());
		}

		BglCloseZipFile_Recursive(i->Left(), removableIds, id);
	}
}

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

		delete[] fileHeader->filename;
		delete[] fileHeader->extraField;
		delete[] fileHeader->data;
		delete[] fileHeader->id;

		delete fileHeader;
	}
}

void BglExtractAllFiles(const char* archivePath)
{
	FILE* archiveFile = fopen(archivePath, "rb");

	assert(archiveFile);

	while (!feof(archiveFile))
	{
		LocalFileHeader fileHeader;
		fread(&fileHeader.signature, sizeof(fileHeader.signature), 1, archiveFile);
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
			fileHeader.filename = new char[fileHeader.filenameLen + 1];
			fread(fileHeader.filename, fileHeader.filenameLen, 1, archiveFile);
			fileHeader.filename[fileHeader.filenameLen] = 0;
			isDirectory = fileHeader.filename[fileHeader.filenameLen - 1] == '/';
		}
		else
			fileHeader.filename = nullptr;

		if (fileHeader.extraFieldLen > 0)
		{
			fileHeader.extraField = new char[fileHeader.extraFieldLen + 1];
			fread(fileHeader.extraField, fileHeader.extraFieldLen, 1, archiveFile);
			fileHeader.extraField[fileHeader.extraFieldLen] = 0;
		}
		else
			fileHeader.extraField = nullptr;

		if (isDirectory)
		{
			BglMakeDirectory(fileHeader.filename);
		}
		else if (fileHeader.uncompressedSize > 0)
		{
			fileHeader.data = new char[fileHeader.uncompressedSize + 1];
			fread(fileHeader.data, fileHeader.uncompressedSize, 1, archiveFile);
			fileHeader.data[fileHeader.uncompressedSize] = 0;
		}
		else
		{
			fileHeader.data = nullptr;
		}

		if (fileHeader.filename && fileHeader.data)
		{
			FILE* file = fopen(fileHeader.filename, "wb");
			assert(file);
			fwrite(fileHeader.data, fileHeader.uncompressedSize, 1, file);
			fclose(file);
		}

		delete[] fileHeader.filename;
		delete[] fileHeader.extraField;
		delete[] fileHeader.data;
	}

	fclose(archiveFile);
}

bool BglFileLoaded(const char* name)
{
	auto slashifiedName = std::string(name);
	BglSlashify(slashifiedName);
	const auto* header = files.LookupData(slashifiedName.c_str());

	return header;
}

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

void BglCloseAllFiles(BTree<LocalFileHeader*>* files)
{
	if (!files)
		return;

	BglCloseAllFiles(files->Left());
	BglCloseAllFiles(files->Right());
	const auto* fileHeader = files->GetData();

	if (fileHeader)
	{
		delete[] fileHeader->filename;
		delete[] fileHeader->extraField;
		delete[] fileHeader->data;
		delete[] fileHeader->id;
		delete fileHeader;
	}

	files->Empty();
}

void BglCloseAllFiles()
{
	BglCloseAllFiles(&files);
}

DArray<const char*>* BglListFiles(char const* apppath, char const* dir, char const* query)
{
	auto queryDir = std::format("{}{}", apppath, dir);
	BglSlashify(queryDir);

	auto* filenames = files.ConvertIndexToDArray();

	for (size_t index = 0; index < filenames->Size(); index++)
	{
		if (!filenames->ValidIndex(index))
			continue;

		const auto* filename = filenames->GetData(index);
		auto fileDir = std::string(filename).substr(0, queryDir.size());

		if (strcmp(fileDir.c_str(), queryDir.c_str()) != 0 || !strstr(filename, query))
			filenames->RemoveData(index);
	}

	return filenames;
}
