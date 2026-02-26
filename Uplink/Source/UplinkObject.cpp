#include "UplinkObject.hpp"

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
