#include "Redshirt/Redshirt.hpp"

#include <cassert>
#include <cstring>
#include <dirent.h>
#include <ostream>
#include <print>
#include <unistd.h>
#include <bits/forward_list.h>
#include <openssl/evp.h>
#include <sys/stat.h>

#include "_.hpp"

typedef void (*FilterFunc)(uint8_t* buffer, size_t size);
typedef bool (*ReadWriteFunc)(FILE* file);

static constexpr size_t HASH_RESULT_SIZE = 20;

static bool rsInitialized = false;
std::string rsapppath;
std::string tempdir;
std::string tempfilename;

static EVP_MD_CTX* HashInitial()
{
	const auto context = EVP_MD_CTX_create();
	EVP_DigestInit_ex(context, EVP_sha1(), nullptr);
	return context;
}

static void HashData(EVP_MD_CTX* context, const uint8_t* data, const size_t size)
{
	EVP_DigestUpdate(context, data, size);
}

static void HashFinal(EVP_MD_CTX* context, uint8_t* buffer, size_t size)
{
	unsigned int outSize = 0;
	EVP_DigestFinal_ex(context, buffer, &outSize);
	EVP_MD_CTX_destroy(context);

	if (size > HASH_RESULT_SIZE)
		size = HASH_RESULT_SIZE;

	// Replicate the behavior of the original code which swaps endianness of each 4-byte chunk
	for (size_t i = 0; i < size; i += 4)
	{
		std::swap(buffer[i + 0], buffer[i + 3]);
		std::swap(buffer[i + 1], buffer[i + 2]);
	}
}

static bool noHeader(FILE*)
{
	return true;
}

static bool readRsEncryptedHeader(FILE* file)
{
	char magic[0x9];

	if (fread(&magic, 9, 1, file) != 1)
		return false;

	uint8_t hash[HASH_RESULT_SIZE];
	if (strcmp(magic, "REDSHRT2") == 0 && fread(hash, 20, 1, file) == 1)
		return true;

	return strcmp(magic, "REDSHIRT") == 0;
}

static bool writeRsEncryptedHeader(FILE* file)
{
	if (fwrite("REDSHRT2", 9, 1, file) != 1)
		return false;

	constexpr uint8_t buf[HASH_RESULT_SIZE] = { };
	return fwrite(buf, HASH_RESULT_SIZE, 1, file) == 1;
}

static bool writeRsEncryptedCheckSum(FILE* file)
{
	fseek(file, 9 + HASH_RESULT_SIZE, 0);

	uint8_t checksum[HASH_RESULT_SIZE];
	RsFileCheckSum(file, checksum, HASH_RESULT_SIZE);
	fseek(file, 9, 0);
	return fwrite(checksum, 0x14, 1, file) == 1;
}

static void encryptBuffer(uint8_t* buffer, const size_t size)
{
	for (size_t i = 0; i < size; i++)
		buffer[i] ^= 0x80;
}

static void decryptBuffer(uint8_t* buffer, const size_t size)
{
	for (size_t i = 0; i < size; i++)
		buffer[i] ^= 0x80;
}

static bool filterStream(FILE* file, FILE* destFile, const FilterFunc filterFunc)
{
	uint8_t buf[0x4000];
	while (true)
	{
		const auto readCount = fread(buf, 1, 0x4000, file);

		if (!readCount)
			return true;

		filterFunc(buf, readCount);
		const auto writeCount = fwrite(buf, 1, readCount, destFile);
		if (writeCount < readCount)
			return false;
	}
}

static bool filterFile(const char* path, const char* destPath,
                       const ReadWriteFunc readHeaderFunc,
                       const ReadWriteFunc writeHeaderFunc,
                       const ReadWriteFunc writeChecksumFunc,
                       const FilterFunc filterFunc)
{
	const auto file = fopen(path, "rb");

	if (!file)
		return false;

	if (!readHeaderFunc(file))
	{
		printf("redshirt: failed to read header!");
		fclose(file);
		return false;
	}

	auto* const destFile = fopen(destPath, "w+b");

	if (!destFile)
	{
		fclose(file);
		return false;
	}

	if (!writeHeaderFunc(destFile))
	{
		std::println("redshirt: failed to write header!");
		goto fail;
	}

	if (!filterStream(file, destFile, filterFunc))
	{
		std::println("redshirt: failed to write content!");
		goto fail;
	}

	if (writeChecksumFunc(destFile))
	{
		std::println("redshirt: failed to write checksum!");
		goto fail;
	}

	fclose(file);
	fclose(destFile);
	return true;

fail:
	fclose(file);
	fclose(destFile);
	remove(destPath);
	return false;
}

static bool filterFileInPlace(const char* path, const char* ext,
                              const ReadWriteFunc readHeaderFunc,
                              const ReadWriteFunc writeHeaderFunc,
                              const ReadWriteFunc writeChecksumFunc,
                              const FilterFunc filterFunc)
{
	const auto destPath = std::format("{}{}", path, ext);
	if (!filterFile(path, destPath.c_str(), readHeaderFunc, writeHeaderFunc, writeChecksumFunc, filterFunc))
	{
		std::println("Redshirt ERROR : Failed to write output file");
		return false;
	}

	remove(path);
	rename(destPath.c_str(), path);
	return true;
}

bool RsFileExists(const char* path)
{
	auto* const file = fopen(path, "r");

	if (file == nullptr)
		return false;

	fclose(file);
	return true;
}

bool RsMakeDirectory(char const* path)

{
	return mkdir(path, 0700) == 0;
}

void RsDeleteDirectory(const char* path)
{
	rmdir(path);
}

const char* RsBasename(const char* path)
{
	char const* result = path;

	for (const char* nextSlashIndex; true; result = nextSlashIndex + 1)
	{
		nextSlashIndex = strchr(result, 0x2f);
		if (nextSlashIndex)
			continue;

		nextSlashIndex = strchr(result, 0x5c);
		if (nextSlashIndex)
			continue;

		return result;
	}
}

void RsFileCheckSum(FILE* file, uint8_t* buffer, const size_t size)
{
	auto* const context = HashInitial();

	const auto buf = new uint8_t[0x4000];
	while (true)
	{
		const auto readCount = fread(buf, 1, 0x4000, file);
		if (readCount == 0)
			break;

		HashData(context, buf, readCount);
	}
	delete[] buf;

	HashFinal(context, buffer, size);
}

bool RsFileEncryptedNoVerify(const char* path)
{
	auto* const file = fopen(path, "rb");

	if (!file)
		return false;

	const auto result = readRsEncryptedHeader(file);
	fclose(file);
	return result;
}

bool RsFileEncrypted(const char* path)
{
	auto* const file = fopen(path, "rb");

	if (!file)
		return false;

	auto success = false;

	char magic[9];
	if (fread(magic, 9, 1, file) != 1)
		goto end;

	if (strcmp(magic, "REDSHIRT") == 0)
	{
		success = true;
		goto end;
	}

	if (strcmp(magic, "REDSHRT2") == 0)
	{
		uint8_t readChecksum[HASH_RESULT_SIZE];
		if (fread(readChecksum, HASH_RESULT_SIZE, 1, file) != 1)
			goto end;

		uint8_t computedChecksum[HASH_RESULT_SIZE];
		RsFileCheckSum(file, computedChecksum, HASH_RESULT_SIZE);

		success = memcmp(readChecksum, computedChecksum, HASH_RESULT_SIZE) == 0;
	}

end:
	fclose(file);
	return success;
}

FILE* RsFileOpen(const char* path, const char* mode)
{
	if (!RsFileExists(path))
		return nullptr;

	if (!RsFileEncrypted(path))
		return fopen(path, mode);

	const auto decryptedFilePath = std::format("{}{}.d", tempdir, RsBasename(path));
	if (filterFile(path, decryptedFilePath.c_str(), readRsEncryptedHeader, noHeader, noHeader, decryptBuffer) != 0)
		return fopen(decryptedFilePath.c_str(), mode);

	std::println("Redshirt ERROR : Failed to write to output file");
	return nullptr;
}

void RsFileClose(const char* path, FILE* file)
{
	fclose(file);

	// TODO: investigate: why not the same as in RsFileOpen?
	const auto decryptedFilePath = std::format("{}.d", path);
	remove(decryptedFilePath.c_str());
}

bool RsEncryptFile(const char* path)
{
	return filterFileInPlace(path, ".e", noHeader, writeRsEncryptedHeader, writeRsEncryptedCheckSum, encryptBuffer);
}

bool RsDecryptFile(const char* path)
{
	if (!RsFileEncrypted(path))
		return true;

	return filterFileInPlace(path, ".d", readRsEncryptedHeader, noHeader, noHeader, decryptBuffer);
}

void RsInitialise(const char* appPath)
{
	rsapppath = appPath;
	if (rsapppath[rsapppath.size() - 1] != '/')
	{
		rsapppath += '/';
	}

	tempdir = "/tmp/uplink-XXXXXX";

	if (!mkdtemp(tempdir.data()))
	{
		std::println("Failed to make temporary directory");
		abort();
	}

	rsInitialized = true;
	tempdir += '/';
	atexit(RsCleanUp);
}

void RsCleanUp()
{
	if (!rsInitialized)
		return;

	rsInitialized = false;
	auto* const dir = opendir(tempdir.c_str());
	if (dir)
	{
		for (auto* dirent = readdir(dir); dirent; dirent = readdir(dir))
			remove(std::format("{}{}", tempdir, dirent->d_name).c_str());
	}

	RsDeleteDirectory(tempdir.c_str());
	BglCloseAllFiles();
}

bool RsLoadArchive(const char* id)
{
	auto path = std::format("{}{}", rsapppath, id);
	auto* file = RsFileOpen(path.c_str(), "rb");

	if (!file)
	{
		const size_t rsapppathLen = rsapppath.size();

		if (rsapppathLen <= 4)
			return false;

		auto index = rsapppath.find("/lib/");

		if (index == std::string::npos)
			index = rsapppath.find("/LIB/");
		if (index == std::string::npos)
			index = rsapppath.find("\\lib\\");
		if (index == std::string::npos)
			index = rsapppath.find("\\LIB\\");

		path = std::format("{}{}", rsapppath.substr(0, index + 1), id);
		file = RsFileOpen(path.c_str(), "rb");
		if (!file)
			return false;
	}

	const auto success = BglOpenZipFile(file, rsapppath.c_str(), id);
	RsFileClose(id, file);

	if (!success)
		std::println("Failed to load data archive {}", id);
	else
		std::println("Successfully loaded data archive {}", id);

	return success;
}

void RsCloseArchive(const char* id)
{
	BglCloseZipFile(id);
}

bool RsArchiveFileLoaded(const char* filename)
{
	const auto path = std::format("{}{}", rsapppath, filename);

	if (RsFileExists(path.c_str()))
		return true;

	return BglFileLoaded(path.c_str());
}

DArray<const char*>* RsListArchive(const char* dir, const char* query)
{
	auto* const result = BglListFiles(rsapppath.c_str(), dir, query);

	for (size_t i = 0; i < result->Size(); i++)
	{
		if (!result->ValidIndex(i))
			continue;

		const auto* const filenameWithApppath = result->GetData(i);

		auto* const filename = new char[strlen(filenameWithApppath) - rsapppath.size() + 1];
		strcpy(filename, filenameWithApppath + rsapppath.size());
		result->PutData(filename, i);
	}

	return result;
}

const char* RsArchiveFileOpen(const char* filename)
{
	const auto path = std::format("{}{}", rsapppath, filename);

	if (RsFileExists(path.c_str()))
	{
		tempfilename = path;
		return tempfilename.c_str();
	}

	if (BglFileLoaded(path.c_str()))
	{
		const auto* ext = strrchr(path.c_str(), '.');
		assert(ext);

		for (size_t i = 0; i < 3; i++)
		{
			const auto extractedPath = std::format("{}temp{}{}", tempdir, i, ext);

			if (BglExtractFile(path.c_str(), extractedPath.c_str()))
			{
				tempfilename = extractedPath;
				return tempfilename.c_str();
			}
		}
	}

	std::println("REDSHIRT : Failed to load file : {}", path);
	return nullptr;
}

FILE* RsArchiveFileOpen(const char* filename, const char* mode)
{
	const auto path = RsArchiveFileOpen(filename);
	if (!path)
		return nullptr;

	return fopen(path, mode);
}

void RsArchiveFileClose(const char* filename, FILE* file)
{
	if (file)
		fclose(file);

	const auto* const ext = strrchr(filename, '.');
	assert(ext);

	// TODO: not use temp0.xxx/temp1.xxx/temp2.xxx as filenames for opened archives maybe?? this sucks.
	for (size_t i = 0; i < 3; i++)
		remove(std::format("{}temp{}{}", tempdir, i, ext).c_str());
}
