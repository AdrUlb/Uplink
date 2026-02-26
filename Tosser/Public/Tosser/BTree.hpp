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
	BTree() = default;

	BTree(const char* name, const T& data)
	{
		_name = new char[strlen(name) + 1];
		strcpy(_name, name);
		_data = data;
	}

	BTree(BTree const& tree) = delete;
	BTree(BTree&& tree) = delete;
	BTree& operator=(const BTree&) = delete;

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
		delete[] _name;
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
			if (cmp == 0) // Found?
				return tree;

			if (cmp < 0 && tree->_left) // Should traverse left?
			{
				tree = tree->_left;
				continue;
			}

			if (cmp > 0 && tree->_right) // Should traverse right?
			{
				tree = tree->_right;
				continue;
			}

			break; // Not found.
		}

		return nullptr;
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

	void RemoveData(const char* name)
	{
		auto tree = this;
		assert(name);
		while (true)
		{
			const auto cmp = strcmp(name, tree->_name);

			// NOTE: the original code had special handling for leaf nodes here, this has been removed for readability

			if (cmp < 0) // Should traverse left?
			{
				if (!tree->_left) // Cannot move left?
					return;

				tree = tree->_left;
				continue;
			}

			if (cmp > 0) // Should traverse right?
			{
				if (!tree->_right) // Cannot move right?
					return;

				tree = tree->_right;
				continue;
			}

			// This is the target node!
			delete tree->_name; // NOTE: the original code did not delete the old name -> memory leak!

			if (tree->_left) // There is a node to the left?
			{
				const auto left = tree->_left;
				const auto right = tree->_right;

				// Copy left node's data into this node
				tree->_name = new char[strlen(left->_name) + 1];
				strcpy(tree->_name, left->_name);

				tree->_data = left->_data;
				tree->_right = left->_right;
				tree->_left = left->_left;

				delete left; // NOTE: the original code did not delete the node -> memory leak!

				// Append old right node again
				if (right) // NOTE: the original code did not bother to check for NULL here, which doesn't break anything but it's unnecessary
					tree->AppendRight(right);
			}
			else if (tree->_right) // There is a node to the right?
			{
				const auto left = tree->_left;
				const auto right = tree->_right;

				// Copy right node's data into this node
				tree->_name = new char[strlen(right->_name) + 1];
				strcpy(tree->_name, right->_name);

				tree->_data = right->_data;
				tree->_left = right->_left;
				tree->_right = right->_right;

				delete right; // NOTE: the original code did not delete the node -> memory leak!
			}
			else
			{
				tree->_name = nullptr;
			}
		}
	}

	void AppendRight(BTree* other)
	{
		auto tree = this;

		while (tree->_right)
			tree = tree->_right;

		tree->_right = other;
	}
};

// For testing
//template class BTree<int>;
//template class BTree<void*>;
