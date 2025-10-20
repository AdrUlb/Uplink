#pragma once

#include <cassert>
#include <cstring>
#include "DArray.hpp"

template<typename T> class BTree
{
	BTree* left_ = nullptr;
	BTree* right_ = nullptr;
	T data_;
	char* label_;

	static void RecursiveConvertToDArray(DArray<T>* const darray, const BTree* tree)
	{
		assert(darray);

		for (auto* current = tree; current; current = current->Right())
		{
			if (current->label_)
				darray->PutData(current->data_);

			RecursiveConvertToDArray(darray, current->Left());
		}
	}

	static void RecursiveConvertIndexToDArray(DArray<const char*>* const darray, const BTree* tree)
	{
		assert(darray);

		for (auto* current = tree; current; current = current->Right())
		{
			if (current->label_)
				darray->PutData(current->label_);

			RecursiveConvertIndexToDArray(darray, current->Left());
		}
	}

public:
	BTree() : data_(0), label_(nullptr) {}

	BTree(const char* label, const T& data) : data_(data)
	{
		label_ = new char[strlen(label) + 1];
		strcpy(label_, label);
	}

	BTree(const BTree&) = delete;
	BTree& operator=(const BTree&) = delete;

	~BTree()
	{
		Empty();
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

		while (current->label_)
		{
			const auto cmp = strcmp(label, current->label_);

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

		current->label_ = new char[strlen(label) + 1];
		strcpy(current->label_, label);
		current->data_ = data;
	}

	const BTree* LookupTree(const char* label) const
	{
		auto* current = this;

		while (current->label_)
		{
			const auto cmp = strcmp(label, current->label_);

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
		delete[] label_;

		left_ = nullptr;
		right_ = nullptr;
		label_ = nullptr;
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
