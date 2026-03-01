#include "Util.hpp"

#include "UplinkObject.hpp"

static constexpr int BTREE_ITEMS_MAX = 0x40000;
static constexpr int DYNAMIC_STRING_MAX = 0x40000;

bool FileReadDataInt(const char* sourceFile, const int sourceLine, void* ptr, const size_t size, const size_t count, FILE* file)
{
	const size_t readCount = fread(ptr, size, count, file);

	return PrintAbort(
		readCount == count, "WARNING: FileReadDataInt, request read count is different then the readed count, request={}, readed={}, errno={}, {}:{}",
		count, readCount, errno, sourceFile, sourceLine
	);
}

bool LoadDynamicStringInt(const char* sourceFile, const int sourceLine, char*& buffer, FILE* file)
{
	buffer = nullptr;

	int size;
	if (!LoadData(&size, 4, file))
		return false;

	if (size == -1)
		return true;

	if (PrintAbort(size <= DYNAMIC_STRING_MAX, "WARNING: LoadDynamicString, size appears to be wrong, size={}, {}:{}", size, sourceFile, sourceLine))
		return false;

	buffer = new char[size + 1];
	if (!LoadFixedString(buffer, size, file))
	{
		buffer[size] = 0;
		return false;
	}

	buffer[size] = 0;
	return true;
}

void SaveDynamicString(const char* buffer, int maxSize, FILE* file)
{
	if (!buffer)
	{
		constexpr int size = -1;
		fwrite(&size, 4u, 1u, file);
		return;
	}

	const auto effectiveMaxSize = maxSize > 0 && maxSize <= DYNAMIC_STRING_MAX ? maxSize : DYNAMIC_STRING_MAX;

	const auto size = static_cast<int>(strlen(buffer)) + 1;
	auto effectiveSize = size;

	if (PrintAbort(size <= effectiveMaxSize,
	               "WARNING: SaveDynamicString, size appears to be too long, size={}, maxsize={}, absolute maxsize={}", size, maxSize, DYNAMIC_STRING_MAX))
		effectiveSize = effectiveMaxSize;

	SaveData(&effectiveSize, 4, file);

	if (effectiveSize > 1)
		SaveFixedString(buffer, effectiveSize - 1, file);

	fputc(0, file);
}

bool LoadBTree(BTree<UplinkObject*>* tree, FILE* file)
{
	PrintAssert(tree);

	uint32_t size;
	if (!LoadData(&size, 4, file))
		return false;

	if (PrintAbort(size <= BTREE_ITEMS_MAX, "WARNING: LoadBTree, number of items appears to be wrong, size={}", size))
		return false;

	if (size == 0)
		return true;

	for (uint32_t i = 0; i < size; i++)
	{
		char* id = nullptr;
		if (!LoadDynamicString(id, file))
			return false;

		if (PrintAbort(id, "WARNING: LoadBTree NULL id"))
			return false;

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

void SaveBTree(BTree<UplinkObject*>* tree, FILE* file)
{
	UplinkAssert(tree);

	const auto items = tree->ConvertToDArray();
	const auto ids = tree->ConvertIndexToDArray();
	const auto count = items->Size();

	int validCount = 0;
	for (auto i = 0; i < count; i++)
		if (items->ValidIndex(i))
			validCount++;

	PrintAbort(validCount <= BTREE_ITEMS_MAX, "WARNING: SaveBTree, number of items appears to be too big, size=%d, maxsize=%d", validCount, BTREE_ITEMS_MAX);
	SaveData(&validCount, 4, file);

	validCount = 0;

	for (int i = 0; i < count && validCount <= BTREE_ITEMS_MAX; i++)
	{
		if (!items->ValidIndex(i))
			continue;

		UplinkAssert(ids->ValidIndex(i));
		const auto id = ids->GetData(i);
		UplinkAssert(id);
		SaveDynamicString(id, file);

		const auto obj = items->GetData(i);
		const auto objectId = obj->GetOBJECTID();
		UplinkAssert(objectId != UplinkObjectID::None);
		SaveData(&objectId, 4, file);
		items->GetData(i)->Save(file);
		validCount++;
	}

	delete items;
	delete ids;
}

void PrintBTree(BTree<UplinkObject*>* tree)
{
	UplinkAssert(tree);

	const auto v2 = tree->ConvertToDArray();
	const auto v3 = tree->ConvertIndexToDArray();

	for (int v1 = 0; v1 < v2->Size(); v1++)
	{
		if (!v2->ValidIndex(v1))
			continue;

		UplinkAssert(!v3->ValidIndex( v1));
		std::println("Index = {}", v3->GetData(v1));

		const auto obj = v2->GetData(v1);

		if (!obj)
		{
			std::println("NULL");
			continue;
		}

		obj->Print();
	}

	delete v2;
	delete v3;
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
