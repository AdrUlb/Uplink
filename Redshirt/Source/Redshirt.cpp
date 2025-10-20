#include "../Include/Redshirt/Redshirt.hpp"

#include <cstdint>
#include <cstring>
#include <bits/forward_list.h>
#include <openssl/evp.h>

#include "../../_.hpp"

static constexpr size_t HASH_RESULT_SIZE = 20;

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

static void HashFinal(EVP_MD_CTX* context, uint8_t* buffer, const size_t size)
{
	unsigned int outSize = 0;
	EVP_DigestFinal_ex(context, buffer, &outSize);
	EVP_MD_CTX_destroy(context);

	// Replicate the behavior of the original code which swaps endianness of each 4-byte chunk
	for (size_t i = 0; i < size; i += 4)
	{
		std::swap(buffer[i + 0], buffer[i + 3]);
		std::swap(buffer[i + 1], buffer[i + 2]);
	}
}

bool readRsEncryptedHeader(FILE* file)
{
	char magic[0x9];

	if (fread(&magic, 9, 1, file) != 1)
		return false;

	uint8_t hash[HASH_RESULT_SIZE];
	if (strcmp(magic, "REDSHRT2") == 0 && fread(hash, 20, 1, file) == 1)
		return true;

	return strcmp(magic, "REDSHIRT") == 0;
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
		uint8_t readHash[HASH_RESULT_SIZE];
		if (fread(readHash, HASH_RESULT_SIZE, 1, file) != 1)
			goto end;

		uint8_t computedHash[HASH_RESULT_SIZE];
		RsFileCheckSum(file, computedHash, HASH_RESULT_SIZE);

		success = memcmp(readHash, computedHash, HASH_RESULT_SIZE) == 0;
	}

end:
	fclose(file);
	return success;
}

FILE* RsFileOpen(const char* path, const char* mode)
{
	TODO_ABORT;
}

void RsFileClose(const char* path, FILE* file)
{
	TODO_ABORT;
}

const char* RsArchiveFileOpen(const char* path)
{
	TODO_ABORT;
}

void RsArchiveFileClose(const char* path, FILE* file)
{
	TODO_ABORT;
}

bool RsEncryptFile(const char* path)
{
	TODO_ABORT;
}
