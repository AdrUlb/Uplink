#pragma once
#include <cstring>

#include "DArray.hpp"

template<typename T> class BTree
{
	BTree* _left = nullptr;
	BTree* _right = nullptr;
	char* _name = nullptr;
	T _data = { };

	static void RecursiveConvertToDArray(DArray<T>* array, BTree* tree)
	{
		assert(array);

		while (tree)
		{
			if (tree->_name)
				array->PutData(tree->_data);

			RecursiveConvertToDArray(array, tree->_left);
			tree = tree->_right;
		}
	}

public:
	BTree(const char* name, const T& data)
	{
		_name = new char[strlen(name) + 1];
		strcpy(_name, name);
		_data = data;
	}

	~BTree()
	{
		Empty();
	}

	DArray<T>* ConvertToDArray()
	{
		const auto array = new DArray<T>();
		RecursiveConvertToDArray(array, this);
		return array;
	}

	void Empty()
	{
		delete _left;
		delete _right;
		delete _name;
		_left = nullptr;
		_right = nullptr;
		_name = nullptr;
	}

	T GetData(const char* name)
	{
		const auto tree = LookupTree(name);
		if (!tree)
			return { };

		return tree->_data;
	}

	BTree* LookupTree(const char* name)
	{
		auto tree = this;

		while (tree->_name)
		{
			const int cmp = strcmp(name, tree->_name);
			if (cmp == 0)
				break;

			if (cmp < 0 && tree->_left)
			{
				tree = tree->_left;
				continue;
			}

			if (cmp > 0 && tree->_right)
			{
				tree = tree->_right;
				continue;
			}

			return nullptr;
		}

		return tree;
	}

	void PutData(const char* name, const T& data)
	{
		auto tree = this;

		while (true)
		{
			if (!tree->_name)
			{
				tree->_name = new char[strlen(name) + 1];
				strcpy(tree->_name, name);
				tree->_data = data;
				return;
			}

			if (strcmp(name, tree->_name) > 0)
			{
				if (!tree->_right)
				{
					tree->_right = new BTree(name, data);
					return;
				}

				tree = tree->_right;
				continue;
			}

			if (!tree->_left)
			{
				tree->_left = new BTree(name, data);
				return;
			}

			tree = tree->_left;
		}
	}
};

// For testing
//template class BTree<int>;
//template class BTree<void*>;
