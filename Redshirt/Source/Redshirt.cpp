#include "../Include/Redshirt/Redshirt.hpp"

#include <cstdint>
#include <cstring>
#include <ostream>
#include <print>
#include <bits/forward_list.h>
#include <openssl/evp.h>

#include "../../_.hpp"

typedef void (*FilterFunc)(uint8_t* buffer, size_t size);
typedef bool (*ReadWriteFunc)(FILE* file);

static constexpr size_t HASH_RESULT_SIZE = 20;

static char tempdir[0x100] { };
static char tempfilename[0x100] { };

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

static bool noHeader(FILE* file)
{
	(void)file;
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
		std::println("redshirt: failed to write containning bytes!");
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

const char* RsArchiveFileOpen(const char* path)
{
	TODO_ABORT;
}

void RsArchiveFileClose(const char* path, FILE* file)
{
	TODO_ABORT;
}
