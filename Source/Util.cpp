#include "Util.hpp"

#include <print>

static constexpr size_t BTREE_ITEMS_MAX = 0x40000;

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

bool LoadDynamicStringInt(const std::source_location loc, char*& ptr, FILE* file)
{
	ptr = nullptr;

	uint32_t size;
	if (!FileReadData(&size, 4, 1, file))
		return false;

	if (size == UINT32_MAX)
		return true;

	if (size > 0x4000)
	{
		std::print("WARNING: LoadDynamicString, size appears to be wrong, size={}, {}:{}",
		           size, loc.line(), loc.column());
		putchar(0xa);
		return false;
	}

	ptr = new char[size + 1];
	const auto success = FileReadData(ptr, size, 1, file);
	ptr[size] = 0;
	return success;
}

bool LoadDynamicStringInt(const std::source_location loc, char* buffer, size_t max, FILE* file)
{
	if (!buffer)
		return false;

	if (max > 0)
		buffer[0] = 0;

	uint32_t size;
	if (!FileReadData(&size, sizeof(size), 1, file))
		return false;

	if (size == UINT32_MAX)
	{
		std::println("WARNING: LoadDynamicString, empty string, {}:{}", loc.line(), loc.column());
		return false;
	}

	if (size > max)
	{
		std::println("WARNING: LoadDynamicString, size > maxsize, size={}, maxsize={}, {}:{}", size, max, loc.line(), loc.column());

		if (max > 0)
		{
			if (!FileReadData(buffer, max, 1, file))
			{
				buffer[max - 1] = 0;
				return false;
			}

			buffer[max - 1] = 0;
			std::println("WARNING: LoadDynamicString, possible string={}, {}:{}", buffer, loc.line(), loc.column());
		}

		return false;
	}

	const auto success = FileReadData(buffer, size, 1, file);
	buffer[size - 1] = 0;
	return success;
}

void SaveDynamicString(const char* str, FILE* file)
{
	SaveDynamicString(str, SIZE_MAX, file);
}

void SaveDynamicString(const char* str, size_t max, FILE* file)
{
	uint32_t buf;

	if (!str)
	{
		buf = UINT32_MAX;
		fwrite(&buf, sizeof(buf), 1, file);
		return;
	}

	if (max > 0x4000)
		max = 0x4000;

	auto size = static_cast<uint32_t>(strlen(str) + 1);

	if (size > max)
	{
		size = max;
		std::println("WARNING: SaveDynamicString, size appears to be too long, size={}, maxsize={}, absolute maxsize={}", buf, max, 0x4000);
	}

	fwrite(&size, sizeof(size), 1, file);
	fwrite(str, size, 1, file);
}

bool LoadBTree(BTree<UplinkObject*>& btree, FILE* file)
{
	uint32_t itemCount;
	if (!FileReadData(&itemCount, sizeof(itemCount), 1, file))
		return false;

	if (itemCount > BTREE_ITEMS_MAX)
	{
		std::println("WARNING: LoadBTree, number of items appears to be wrong, size={}", itemCount);
		return false;
	}

	for (size_t i = 0; i < itemCount; i++)
	{
		char* label = nullptr;
		if (!LoadDynamicString(label, file))
			return false;

		if (!label)
		{
			std::println("WARNING: LoadBTree NULL id");
			return false;
		}

		UplinkObjectId id;
		static_assert(sizeof(id) == 4);
		if (!FileReadData(&id, sizeof(id), 1, file))
			return false;

		const auto obj = CreateUplinkObject(id);
		if (!obj || !obj->Load(file))
		{
			delete[] label;
			delete obj;
			return false;
		}

		btree.PutData(label, obj);
		delete[] label;
	}

	return true;
}

void SaveBTree(const BTree<UplinkObject*>& btree, FILE* file)
{
	const auto* darray = btree.ConvertToDArray();
	const auto* labels = btree.ConvertIndexToDArray();
	const auto size = darray->Size();

	uint32_t itemCount = 0;

	if (size <= 0)
	{
		fwrite(&itemCount, 4, 1, file);
		itemCount = 0;
	}
	else
	{
		for (size_t i = 0; i < size; i++)
			if (darray->ValidIndex(i))
				itemCount++;

		if (itemCount > BTREE_ITEMS_MAX)
		{
			std::println("WARNING: SaveBTree, number of items appears to be too big, size={}, maxsize={}", itemCount, BTREE_ITEMS_MAX);
			itemCount = BTREE_ITEMS_MAX;
		}

		fwrite(&itemCount, sizeof(itemCount), 1, file);

		itemCount = 0;
		for (int32_t i = 0; i < size && itemCount < BTREE_ITEMS_MAX; i++)
		{
			if (!darray->ValidIndex(i))
				continue;

			UplinkAssert(labels->ValidIndex(i));

			const auto data = darray->GetData(i);
			UplinkAssert(data);

			SaveDynamicString(labels->GetData(i), file);

			const auto obj = darray->GetData(i);
			const auto id = obj->GetOBJECTID();
			UplinkAssert(id != UplinkObjectId::None);

			static_assert(sizeof(UplinkObjectId) == 4);
			fwrite(&id, sizeof(id), 1, file);

			obj->Save(file);
			itemCount++;
		}
	}

	delete darray;
	delete labels;
}

void PrintBTree(const BTree<UplinkObject*>& btree)
{
	const auto* const darray = btree.ConvertToDArray();
	const auto* const labels = btree.ConvertIndexToDArray();

	for (size_t i = 0; i < darray->Size(); i++)
	{
		if (!darray->ValidIndex(i))
			continue;

		UplinkAssert(labels->ValidIndex(i));

		std::println("Index = {}", labels->GetData(i));

		const auto data = darray->GetData(i);
		if (!data)
		{
			std::println("NULL");
			continue;
		}

		data->Print();
	}

	delete labels;
	delete darray;
}

void DeleteBTreeData(const BTree<UplinkObject*>& btree)
{
	const auto* const darray = btree.ConvertToDArray();

	for (size_t i = 0; i < darray->Size(); i++)
	{
		if (!darray->ValidIndex(i))
			continue;

		delete darray->GetData(i);
	}

	delete darray;
}

bool LoadBTree(BTree<char*>& btree, FILE* file)
{
	uint32_t itemCount;
	if (!FileReadData(&itemCount, sizeof(itemCount), 1, file))
		return false;

	if (itemCount > 0x40000)
	{
		std::println("WARNING: LoadBTree, number of items appears to be wrong, size={}", itemCount);
		return false;
	}

	for (int32_t i = 0; i < itemCount; i++)
	{
		char* label = nullptr;
		if (!LoadDynamicString(label, file))
			return false;

		if (!label)
		{
			std::println("WARNING: LoadBTree NULL id");
			return false;
		}

		char* value = nullptr;
		if (!LoadDynamicString(value, file))
		{
			delete[] label;
			return false;
		}

		btree.PutData(label, value);
		delete[] label;
	}

	return true;
}

void SaveBTree(const BTree<char*>& btree, FILE* file)
{
	const auto* darray = btree.ConvertToDArray();
	const auto* labels = btree.ConvertIndexToDArray();
	const auto size = darray->Size();

	uint32_t itemCount = 0;

	for (size_t i = 0; i < size; i++)
		if (darray->ValidIndex(i))
			itemCount++;

	if (itemCount > BTREE_ITEMS_MAX)
	{
		std::println("WARNING: SaveBTree, number of items appears to be too big, size={}, maxsize={}", itemCount, BTREE_ITEMS_MAX);
		itemCount = BTREE_ITEMS_MAX;
	}

	fwrite(&itemCount, sizeof(itemCount), 1, file);

	size_t writtenItems = 0;

	for (size_t i = 0; i < size && writtenItems < BTREE_ITEMS_MAX; i++)
	{
		if (!darray->ValidIndex(i))
			continue;

		UplinkAssert(labels->ValidIndex(i));
		SaveDynamicString(labels->GetData(i), file);
		SaveDynamicString(darray->GetData(i), file);
		writtenItems++;
	}

	delete darray;
	delete labels;
}

void PrintBTree(const BTree<char*>& btree)
{
	const auto* const darray = btree.ConvertToDArray();
	const auto* const labels = btree.ConvertIndexToDArray();

	for (size_t i = 0; i < darray->Size(); i++)
	{
		if (!darray->ValidIndex(i))
			continue;

		UplinkAssert(labels->ValidIndex(i));

		std::println("Index = {}", labels->GetData(i));

		const auto data = darray->GetData(i);
		if (!data)
		{
			std::println("NULL");
			continue;
		}

		std::println("{}", data);
	}

	delete labels;
	delete darray;
}

void DeleteBTreeData(const BTree<char*>& btree)
{
	const auto* const darray = btree.ConvertToDArray();

	for (size_t i = 0; i < darray->Size(); i++)
	{
		if (!darray->ValidIndex(i))
			continue;

		delete[] darray->GetData(i);
	}

	delete darray;
}
