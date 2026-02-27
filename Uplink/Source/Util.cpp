#include "Util.hpp"

#include "UplinkObject.hpp"

bool FileReadDataInt(const char* sourceFile, const int sourceLine, void* ptr, const size_t size, const size_t count, FILE* file)
{
	const size_t readCount = fread(ptr, size, count, file);

	PrintAbort(
		readCount == count, "WARNING: FileReadDataInt, request read count is different then the readed count, request={}, readed={}, errno={}, {}:{}",
		count, readCount, errno, sourceFile, sourceLine
	);

	return true;
}

bool LoadDynamicStringInt(const char* sourceFile, const int sourceLine, char*& buffer, FILE* file)
{
	buffer = nullptr;

	int32_t size;
	if (!LoadData(&size, 4, file))
		return false;

	if (size == -1)
		return true;

	PrintAbort(size <= 0x4000, "WARNING: LoadDynamicString, size appears to be wrong, size={}, {}:{}", size, sourceFile, sourceLine);

	buffer = new char[size + 1];
	if (!LoadFixedString(buffer, size, file))
	{
		buffer[size] = 0;
		return false;
	}

	buffer[size] = 0;
	return true;
}

bool LoadBTree(BTree<UplinkObject*>* tree, FILE* file)
{
	PrintAssert(tree);

	uint32_t size;
	if (!LoadData(&size, 4, file))
		return false;

	PrintAbort(size <= 0x40000, "WARNING: LoadBTree, number of items appears to be wrong, size={}", size);

	if (size == 0)
		return true;

	for (uint32_t i = 0; i < size; i++)
	{
		char* id = nullptr;
		if (!LoadDynamicString(id, file))
			return false;

		PrintAbort(id, "WARNING: LoadBTree NULL id");

		uint32_t objectId;
		if (!LoadData(&objectId, 4, file))
			return false;

		const auto obj = CreateUplinkObject(static_cast<UplinkObjectID>(objectId));
		if (!obj || !obj->Load(file))
		{
			delete[] id;
			delete obj;
			return false;
		}

		tree->PutData(id, obj);
		delete[] id;
	}

	return true;
}

void DeleteBTreeData(BTree<UplinkObject*>* tree)
{
	UplinkAssert(tree);

	const auto array = tree->ConvertToDArray();
	for (auto i = 0; i < array->Size(); i++)
	{
		if (array->ValidIndex(i))
			delete array->GetData(i);
	}

	delete array;
}
