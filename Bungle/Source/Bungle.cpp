#include "Bungle.hpp"

#include "Tosser/BTree.hpp"
#include "Tosser/LList.hpp"

struct LocalFileHeader
{
	char Magic[4] { };
	uint16_t Version = 0;
	uint16_t Flags = 0;
	uint16_t CompressionMethod = 0;
	uint16_t ModificationTime = 0;
	uint16_t ModificationDate = 0;
	uint32_t Crc32 = 0;
	uint32_t CompressedSize = 0;
	uint32_t UncompressedSize = 0;
	uint16_t FileNameLength = 0;
	uint16_t ExtraFieldLength = 0;
	char* FileName = nullptr;
	char* ExtraField = nullptr;
	char* Data = nullptr;
	char* Id = nullptr;
};

BTree<LocalFileHeader*> files;

static void BglCloseZipFile_Recursive(BTree<LocalFileHeader*>* files, LList<const char*>* removableIds, const char* id)
{
	assert(removableIds);
	assert(id);

	while (files)
	{
		if (const auto data = files->NodeData(); data && data->Id && strcmp(data->Id, id) == 0)
			removableIds->PutData(files->NodeName());

		BglCloseZipFile_Recursive(files->Left(), removableIds, id);
		files = files->Right();
	}
}

static void BglSlashify(char* path)
{
	for (auto i = path; *i; i++)
	{
		if (*i == '\\')
			*i = '/';

		else if (*i >= 'A' && *i <= 'Z')
			*i = static_cast<char>(*i - 'A' + 'a');
	}
}

static void BglCloseAllFiles(BTree<LocalFileHeader*>* files)
{
	if (!files)
		return;

	BglCloseAllFiles(files->Left());
	BglCloseAllFiles(files->Right());

	if (const auto lfi = files->NodeData())
	{
		delete[] lfi->FileName;
		delete[] lfi->ExtraField;
		delete[] lfi->Data;
		delete[] lfi->Id;
		delete lfi;
	}

	files->Empty();
}

void BglCloseAllFiles()
{
	BglCloseAllFiles(&files);
}

void BglCloseZipFile(const char* id)
{
	LList<const char*> removableIds;
	BglCloseZipFile_Recursive(&files, &removableIds, id);

	for (auto i = 0; i < removableIds.Size(); i++)
	{
		const auto filename = removableIds.GetData(i);
		assert(filename);

		LocalFileHeader* lfi = files.GetData(filename);
		assert(lfi);

		files.RemoveData(filename);
		delete[] lfi->FileName;
		delete[] lfi->ExtraField;
		delete[] lfi->Data;
		delete[] lfi->Id;
		operator delete(lfi);
	}
}

DArray<const char*>* BglListFiles(const char* appPath, const char* dirName, const char* query)
{
	char fileDirPath[256];
	char dirPath[256];

	sprintf(dirPath, "%s%s", appPath, dirName);
	BglSlashify(dirPath);

	const auto list = files.ConvertIndexToDArray();

	for (auto i = 0; i < list->Size(); i++)
	{
		if (!list->ValidIndex(i))
			continue;

		const auto fileName = list->GetData(i);

		strncpy(fileDirPath, fileName, strlen(dirPath));
		fileDirPath[strlen(dirPath)] = 0;

		if (!strstr(fileName, query) || strcmp(fileDirPath, dirPath) != 0)
			list->RemoveData(i);
	}

	return list;
}

bool BglExtractFile(const char* fileName, const char* destPath)
{
	const auto slashified = new char[strlen(fileName) + 1];
	strcpy(slashified, fileName);
	BglSlashify(slashified);

	const auto data = files.GetData(slashified);
	delete[] slashified;

	if (!data)
		return false;

	const auto file = destPath ? fopen(destPath, "wb") : fopen(fileName, "wb");
	if (!file)
		return false;

	fwrite(data->Data, data->UncompressedSize, 1, file);
	fclose(file);

	return true;
}

bool BglFileLoaded(const char* fileName)
{
	const auto slashified = new char[strlen(fileName) + 1];
	strcpy(slashified, fileName);
	BglSlashify(slashified);

	const auto data = files.GetData(slashified);
	delete[] slashified;

	return data != nullptr;
}

bool BglOpenZipFile(FILE* file, const char* appPath, const char* id)
{
	char filePath[256];

	if (!file)
		return false;

	while (!feof(file))
	{
		const auto lfi = new LocalFileHeader();
		fread(lfi, 4, 1, file);

		if (lfi->Magic[0] != 'P' || lfi->Magic[1] != 'K')
		{
			operator delete(lfi);
			return true;
		}

		fread(&lfi->Version, 2, 1, file);
		fread(&lfi->Flags, 2, 1, file);
		fread(&lfi->CompressionMethod, 2, 1, file);
		fread(&lfi->ModificationTime, 2, 1, file);
		fread(&lfi->ModificationDate, 2, 1, file);
		fread(&lfi->Crc32, 4, 1, file);
		fread(&lfi->CompressedSize, 4, 1, file);
		fread(&lfi->UncompressedSize, 4, 1, file);
		fread(&lfi->FileNameLength, 2, 1, file);
		fread(&lfi->ExtraFieldLength, 2, 1, file);

		if (lfi->FileNameLength > 0)
		{
			lfi->FileName = new char[lfi->FileNameLength + 1];
			fread(lfi->FileName, lfi->FileNameLength, 1, file);
			lfi->FileName[lfi->FileNameLength] = 0;
		}

		if (lfi->ExtraFieldLength > 0)
		{
			lfi->ExtraField = new char[lfi->ExtraFieldLength + 1];
			fread(lfi->ExtraField, lfi->ExtraFieldLength, 1, file);
			lfi->ExtraField[lfi->ExtraFieldLength] = 0;
		}

		if (lfi->UncompressedSize > 0)
		{
			lfi->Data = new char[lfi->UncompressedSize + 1];
			fread(lfi->Data, lfi->UncompressedSize, 1, file);
			lfi->Data[lfi->UncompressedSize] = 0;
		}

		if (id)
		{
			lfi->Id = new char[strlen(id) + 1];
			strcpy(lfi->Id, id);
		}

		if (lfi->CompressionMethod != 0 || lfi->CompressedSize != lfi->UncompressedSize || !lfi->FileName)
		{
			delete[] lfi->FileName;
			delete[] lfi->ExtraField;
			delete[] lfi->Data;
			delete[] lfi->Id;
			delete lfi;
			continue;
		}

		sprintf(filePath, "%s%s", appPath, lfi->FileName);
		BglSlashify(filePath);
		files.PutData(filePath, lfi);
	}

	return true;
}

bool BglOpenZipFile(const char* filePath, const char* appPath, const char* id)
{
	const auto file = fopen(filePath, "rb");
	if (!file)
		return false;

	const auto ret = BglOpenZipFile(file, appPath, id);
	fclose(file);
	return ret;
}
