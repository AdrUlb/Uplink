#include "Redshirt.hpp"

#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <unistd.h>
#include <openssl/evp.h>
#include <sys/stat.h>

#include "Bungle.hpp"
#include "Tosser/DArray.hpp"

typedef EVP_MD_CTX* HashContext;

typedef bool (*ReadWriteFunc)(FILE* file);
typedef void (*FilterFunc)(uint8_t* buffer, size_t size);

static constexpr size_t HASH_RESULT_SIZE = 20;

static std::string rsapppath;
static std::string tempdir;
static bool rsInitialised = false;

static HashContext HashInitial()
{
	const auto context = EVP_MD_CTX_create();
	EVP_DigestInit_ex(context, EVP_sha1(), nullptr);
	return context;
}

static void HashData(const HashContext context, const uint8_t* data, const size_t size)
{
	EVP_DigestUpdate(context, data, size);
}

static void HashFinal(const HashContext context, uint8_t* buffer)
{
	unsigned int outSize = 0;
	EVP_DigestFinal_ex(context, buffer, &outSize);
	EVP_MD_CTX_destroy(context);

	// Replicate the behavior of the original code which swaps endianness of each 4-byte chunk
	for (size_t i = 0; i < HASH_RESULT_SIZE; i += 4)
	{
		std::swap(buffer[i + 0], buffer[i + 3]);
		std::swap(buffer[i + 1], buffer[i + 2]);
	}
}

int filterStream(FILE* file, FILE* filteredFile, const FilterFunc filter)
{
	uint8_t buf[0x4000];

	while (true)
	{
		const size_t readCount = fread(buf, 1u, 0x4000u, file);
		if (readCount == 0)
			break;

		filter(buf, readCount);
		if (fwrite(buf, 1, readCount, filteredFile) < readCount)
			return false;
	}

	return true;
}

bool filterFile(
	const char* path, const char* destPath,
	const ReadWriteFunc readHeader,
	const ReadWriteFunc writeHeader,
	const ReadWriteFunc writeChecksum,
	const FilterFunc filter
)
{
	const auto file = fopen(path, "rb");
	if (!file)
		return false;

	if (!readHeader(file))
	{
		std::print("redshirt: failed to read header!");
		fclose(file);
		return false;
	}

	const auto filteredFile = fopen(destPath, "w+b");
	if (!filteredFile)
	{
		fclose(file);
		return false;
	}

	if (!writeHeader(filteredFile))
	{
		printf("redshirt: failed to write header!");
		goto error;
	}

	if (!filterStream(file, filteredFile, filter))
	{
		printf("redshirt: failed to write containning bytes!");
		goto error;
	}

	if (!writeChecksum(filteredFile))
	{
		printf("redshirt: failed to write checksum!");
		goto error;
	}

	fclose(file);
	fclose(filteredFile);
	return true;

error:
	fclose(file);
	fclose(filteredFile);
	remove(destPath);
	return false;
}

static bool filterFileInPlace(
	const char* filePath,
	const char* extension,
	const ReadWriteFunc readHeader,
	const ReadWriteFunc writeHeader,
	const ReadWriteFunc writeChecksum,
	const FilterFunc filter
)
{
	const auto filteredPath = std::format("{}{}", filePath, extension);

	if (!filterFile(filePath, filteredPath.c_str(), readHeader, writeHeader, writeChecksum, filter))
	{
		puts("Redshirt ERROR : Failed to write output file");
		return false;
	}

	remove(filePath);
	rename(filteredPath.c_str(), filePath);
	return true;
}

static bool noHeader(FILE*)
{
	return true;
}

static bool readRsEncryptedHeader(FILE* file)
{
	char ptr[9];
	if (fread(ptr, 9, 1, file) != 1)
		return false;

	if (strcmp(ptr, "REDSHRT2") == 0)
	{
		char buf[HASH_RESULT_SIZE];
		return fread(buf, HASH_RESULT_SIZE, 1, file) == 1;
	}

	return strcmp(ptr, "REDSHIRT") == 0;
}

static bool writeRsEncryptedHeader(FILE* file)
{
	if (fwrite("REDSHRT2", 9u, 1u, file) != 1)
		return false;

	// Write checksum as all zeros to fill in later
	constexpr char v5[HASH_RESULT_SIZE] { };
	return fwrite(v5, HASH_RESULT_SIZE, 1, file) == 1;
}

static bool writeRsEncryptedCheckSum(FILE* stream)
{
	uint8_t buf[HASH_RESULT_SIZE];
	fseek(stream, 9 + HASH_RESULT_SIZE, 0);
	RsFileCheckSum(stream, buf);

	fseek(stream, 9, 0);
	return fwrite(buf, HASH_RESULT_SIZE, 1, stream) == 1;
}

static void decryptBuffer(uint8_t* buffer, const size_t size)
{
	for (size_t i = 0; i < size; i++)
		buffer[i] += 0x80;
}

static void encryptBuffer(uint8_t* buffer, const size_t size)
{
	for (size_t i = 0; i < size; i++)
		buffer[i] += 0x80;
}

static bool RsMakeDirectory(char const* path)
{
	return mkdir(path, 0700) == 0;
}

static void RsDeleteDirectory(const char* path)
{
	rmdir(path);
}

static bool RsFileExists(const char* path)
{
	const auto file = fopen(path, "r");

	if (!file)
		return false;

	fclose(file);
	return true;
}

static const char* RsBasename(const char* path)
{
	while (true)
	{
		auto slashIndex = strchr(path, '/');
		if (!slashIndex)
		{
			slashIndex = strchr(path, '\\');
			if (!slashIndex)
				break;
		}

		path = slashIndex + 1;
	}

	return path;
}

static void RsCleanUp()
{
	if (!rsInitialised)
		return;

	rsInitialised = false;
	const auto dir = opendir(tempdir.c_str());
	if (!dir)
		goto end;

	for (auto entry = readdir(dir); entry; entry = readdir(dir))
	{
		remove(std::format("{}{}", tempdir, entry->d_name).c_str());
	}
	closedir(dir);

end:
	RsDeleteDirectory(tempdir.c_str());
	BglCloseAllFiles();
}

void RsInitialise(const char* appPath)
{
	rsapppath = appPath;
	tempdir = "/tmp/uplink-XXXXXX";

	if (!mkdtemp(tempdir.data()))
	{
		puts("Failed to make temporary directory");
		abort();
	}

	rsInitialised = true;
	tempdir += '/';
	atexit(RsCleanUp);
}

void RsFileCheckSum(FILE* file, uint8_t* buffer)
{
	uint8_t buf[0x4000];

	const auto context = HashInitial();
	while (const auto readSize = fread(buf, 1, sizeof(buf), file))
		HashData(context, buf, readSize);

	HashFinal(context, buffer);
}

bool RsFileEncrypted(const char* path)
{
	const auto file = fopen(path, "rb");
	if (!file)
		return false;

	auto ret = false;

	char ptr[9];
	if (fread(ptr, 9, 1, file) != 1)
		goto end;

	if (strcmp(ptr, "REDSHRT2") == 0)
	{
		uint8_t readHash[HASH_RESULT_SIZE];

		if (fread(readHash, HASH_RESULT_SIZE, 1, file) != 1)
			goto end;

		uint8_t computedHash[HASH_RESULT_SIZE];
		RsFileCheckSum(file, computedHash);

		ret = memcmp(readHash, computedHash, HASH_RESULT_SIZE) == 0;
		goto end;
	}

	if (strcmp(ptr, "REDSHIRT") == 0)
		ret = true;

end:
	fclose(file);
	return ret;
}

bool RsFileEncryptedNoVerify(const char* path)
{
	const auto file = fopen(path, "rb");
	if (!file)
		return false;

	const auto ret = readRsEncryptedHeader(file);
	fclose(file);
	return ret;
}

FILE* RsFileOpen(const char* path, const char* modes)
{
	if (!RsFileExists(path))
		return nullptr;

	if (!RsFileEncrypted(path))
		return fopen(path, modes);

	const auto decryptedPath = std::format("{}{}.d", tempdir, RsBasename(path));
	if (!filterFile(path, decryptedPath.c_str(), readRsEncryptedHeader, noHeader, noHeader, decryptBuffer))
	{
		puts("Redshirt ERROR : Failed to write to output file");
		return nullptr;
	}

	return fopen(decryptedPath.c_str(), modes);
}

void RsFileClose(const char* path, FILE* file)
{
	fclose(file);
	const auto decryptedPath = std::format("{}.d", path);
	remove(decryptedPath.c_str());
}

bool RsLoadArchive(const char* id)
{
	auto filePath = std::format("{}{}", rsapppath, id);
	auto file = RsFileOpen(filePath.c_str(), "rb");
	if (!file)
	{
		const auto apppathLength = rsapppath.length();
		if (apppathLength <= 4)
			return false;

		const auto c0 = rsapppath[apppathLength - 5];
		const auto c1 = rsapppath[apppathLength - 4];
		const auto c2 = rsapppath[apppathLength - 3];
		const auto c3 = rsapppath[apppathLength - 2];
		const auto c4 = rsapppath[apppathLength - 1];

		if (c0 != '\\' && c0 != '/')
			return false;

		if ((c1 != 'l' && c1 != 'L') || (c2 != 'i' && c2 != 'I') || (c3 != 'b' && c3 != 'B') || (c4 != '\\' && c4 != '/'))
			return false;

		filePath.resize(apppathLength - 4);
		filePath += id;

		file = RsFileOpen(filePath.c_str(), "rb");
		if (!file)
			return false;
	}

	const auto ret = BglOpenZipFile(file, rsapppath.c_str(), id);
	RsFileClose(id, file);
	if (ret)
		std::println("Successfully loaded data archive {}", id);
	else
		std::println("Failed to load data archive {}", id);

	return ret;
}

void RsCloseArchive(const char* id)
{
	BglCloseZipFile(id);
}

bool RsArchiveFileLoaded(const char* name)
{
	const auto path = std::format("{}{}", rsapppath, name);
	if (RsFileExists(path.c_str()))
		return true;

	return BglFileLoaded(path.c_str());
}

std::optional<std::string> RsArchiveFileOpen(const char* name)
{
	auto path = std::format("{}{}", rsapppath, name);
	if (RsFileExists(path.c_str()))
		return path;

	if (BglFileLoaded(path.c_str()))
	{
		const auto extensionIndex = path.find_last_of('.');
		assert(extensionIndex != std::string::npos);
		const auto extension = std::string_view(path).substr(extensionIndex);

		// FIXME: this is a TERRIBLE way to do this
		for (auto i = 0; i < 3; i++)
		{
			auto tempPath = std::format("{}temp{}{}", tempdir, i, extension);
			if (BglExtractFile(path.c_str(), tempPath.c_str()))
				return tempPath;
		}
	}

	std::println("REDSHIRT : Failed to load file : {}", path);
	return { };
}

FILE* RsArchiveFileOpen(const char* name, const char* mode)
{
	if (const auto v2 = RsArchiveFileOpen(name))
		return fopen(v2->c_str(), mode);

	return nullptr;
}

void RsArchiveFileClose(std::string_view path, FILE* file)
{
	if (file)
		fclose(file);

	const auto extensionIndex = path.find_last_of('.');
	assert(extensionIndex != std::string::npos);
	const auto extension = std::string_view(path).substr(extensionIndex);

	for (int i = 0; i < 3; i++)
	{
		const auto tempPath = std::format("{}temp{}{}", tempdir, i, extension);
		remove(tempPath.c_str());
	}
}

DArray<const char*>* RsListArchive(const char* dir, const char* query)
{
	const auto array = BglListFiles(rsapppath.c_str(), dir, query);

	for (int i = 0; i < array->Size(); i++)
		if (array->ValidIndex(i))
			array->PutData(array->GetData(i) + rsapppath.length(), i);

	return array;
}

bool RsDecryptFile(const char* path)
{
	return !RsFileEncrypted(path) || filterFileInPlace(path, ".d", readRsEncryptedHeader, noHeader, noHeader, decryptBuffer);
}

bool RsEncryptFile(const char* path)
{
	return filterFileInPlace(path, ".e", noHeader, writeRsEncryptedHeader, writeRsEncryptedCheckSum, encryptBuffer);
}
