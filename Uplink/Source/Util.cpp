#include "Util.hpp"

bool FileReadDataInt(const char* sourceFile, const int sourceLine, void* ptr, const size_t size, const size_t count, FILE* file)
{
	const size_t readCount = fread(ptr, size, count, file);
	if (readCount == count)
		return true;

	std::println(
		"Print Abort: {} ln {} : WARNING: FileReadDataInt, request read count is different then the readed count, request={}, readed={}, errno={}, {}:{}",
		sourceFile, sourceLine,
		count, readCount, errno, sourceFile, sourceLine
	);
	return false;
}
