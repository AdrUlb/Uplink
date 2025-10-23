#pragma once

#include <cassert>
#include <cstring>
#include "DArray.hpp"

template<typename T> class BTree
{
	BTree* left_ = nullptr;
	BTree* right_ = nullptr;
	T data_;
	char* id_;

	static void RecursiveConvertToDArray(DArray<T>* const darray, const BTree* tree)
	{
		assert(darray);

		for (auto* current = tree; current; current = current->Right())
		{
			if (current->id_)
				darray->PutData(current->data_);

			RecursiveConvertToDArray(darray, current->Left());
		}
	}

	static void RecursiveConvertIndexToDArray(DArray<const char*>* const darray, const BTree* tree)
	{
		assert(darray);

		for (auto* current = tree; current; current = current->Right())
		{
			if (current->id_)
				darray->PutData(current->id_);

			RecursiveConvertIndexToDArray(darray, current->Left());
		}
	}

	void AppendRight(BTree* node)
	{
		auto* current = this;

		while (current->Right())
			current = current->Right();

		current->right_ = node;
	}

public:
	BTree() : data_(0), id_(nullptr) {}

	BTree(const char* label, const T& data) : data_(data)
	{
		id_ = new char[strlen(label) + 1];
		strcpy(id_, label);
	}

	BTree(const BTree&) = delete;
	BTree& operator=(const BTree&) = delete;

	~BTree()
	{
		Empty();
	}

	const char* Id() const
	{
		return id_;
	}

	BTree* Left() const
	{
		return left_;
	}

	BTree* Right() const
	{
		return right_;
	}

	void PutData(const char* label, const T& data)
	{
		auto* current = this;

		while (current->id_)
		{
			const auto cmp = strcmp(label, current->id_);

			if (cmp > 0)
			{
				if (!current->right_)
				{
					current->right_ = new BTree(label, data);
					return;
				}

				current = current->right_;
			}
			else
			{
				if (!current->left_)
				{
					current->left_ = new BTree(label, data);
					return;
				}

				current = current->left_;
			}
		}

		current->id_ = new char[strlen(label) + 1];
		strcpy(current->id_, label);
		current->data_ = data;
	}

	const BTree* LookupTree(const char* label) const
	{
		auto* current = this;

		while (current->id_)
		{
			const auto cmp = strcmp(label, current->id_);

			if (cmp == 0)
				return current;

			if (cmp < 0 && current->left_)
			{
				current = current->left_;
				continue;
			}

			if (cmp > 0 && current->right_)
			{
				current = current->right_;
				continue;
			}

			break;
		}

		return nullptr;
	}

	BTree* LookupTree(const char* label)
	{
		return const_cast<BTree*>(static_cast<const BTree*>(this)->LookupTree(label));
	}

	const T& GetData() const
	{
		return data_;
	}

	void RemoveData(const char* label)
	{
		assert(label);

		auto* current = this;
		while (true)
		{
			const auto cmp = strcmp(label, current->id_);

			// FIXME: Why are we additionally checking if the next node to traverse towards is the target node instead of simply letting the next call check

			// Traverse left
			if (cmp < 0)
			{
				// Nowhere to go
				if (!current->Left())
					break;

				// Left node is the target node and does not have any children
				if (strcmp(current->Left()->id_, label) == 0 && !current->Left()->Left() && !current->Left()->Right())
				{
					// Remove node
					// FIXME: delete node before setting to null
					current->left_ = nullptr;
					break;
				}

				current = current->Left();
				continue;
			}

			// Traverse right
			if (cmp > 0)
			{
				// Nowhere to go
				if (!current->Right())
					break;

				// Right node is the target node and does not have any children
				if (!strcmp(current->Right()->id_, label) && !current->Right()->Left() && !current->Right()->Right())
				{
					// Remove node
					// FIXME: delete node before setting to null
					current->right_ = nullptr;
					break;
				}

				current = current->Right();
				continue;
			}

			// cmp == 0, current is target node

			// There is a child node to the left
			if (current->Left())
			{
				auto* rightNode = current->Right();

				// Copy all data from left node into current node
				current->id_ = new char[strlen(current->Left()->id_) + 1];
				strcpy(current->id_, current->Left()->id_);
				current->right_ = current->Left()->Right();
				current->data_ = current->Left()->data_;
				current->left_ = current->Left()->Left();

				// Append original right node
				current->AppendRight(rightNode);
				break;
			}

			// There is a child node to the right (none to the left)
			if (current->Right())
			{
				// Copy all data from right node to this node
				current->id_ = new char[strlen(current->Right()->id_) + 1];
				strcpy(current->id_, current->Right()->id_);

				current->left_ = current->Right()->Left();
				current->data_ = current->Right()->data_;
				current->right_ = current->Right()->Right();
				break;
			}

			current->id_ = nullptr;

			break;
		}
	}

	void SetData(const T& data)
	{
		data_ = data;
	}

	T LookupData(const char* label) const
	{
		const auto node = LookupTree(label);
		if (!node)
			return { };

		return node->data_;
	}

	void Empty()
	{
		delete left_;
		delete right_;
		delete[] id_;

		left_ = nullptr;
		right_ = nullptr;
		id_ = nullptr;
	}

	[[nodiscard]] DArray<T>* ConvertToDArray() const
	{
		auto* const darray = new DArray<T>();
		RecursiveConvertToDArray(darray, this);
		return darray;
	}

	[[nodiscard]] DArray<const char*>* ConvertIndexToDArray() const
	{
		auto* const darray = new DArray<const char*>();
		RecursiveConvertIndexToDArray(darray, this);
		return darray;
	}
};
