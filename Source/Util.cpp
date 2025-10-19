#include "Util.hpp"

#include <print>

bool FileReadDataInt(const std::source_location loc, void* ptr, const size_t size, size_t count, FILE* file)
{
	const auto read = fread(ptr, size, count, file);

	if (read == count)
		return true;

	std::print("Print Abort: {} ln {} : ",
	           loc.file_name(), loc.line());
	std::println("WARNING: FileReadDataInt, requested read count is different from the actual read count, requested=%d, actual=%d, errno=%d, %s:%d",
	             count, read, errno, loc.file_name(), loc.line());

	return false;
}
