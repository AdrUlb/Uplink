#pragma once

#include <cassert>
#include <cstddef>
#include <print>

template<typename T> class DArray
{
	size_t stepSize_ = 1;
	size_t size_ = 0;
	T* data_ = nullptr;
	bool* valid_ = nullptr;

public:
	~DArray()
	{
		Empty();
	}

	[[nodiscard]] size_t StepSize() const
	{
		return stepSize_;
	}

	[[nodiscard]] size_t Size() const
	{
		return size_;
	}

	[[nodiscard]] bool ValidIndex(size_t index) const
	{
		return index < size_ && valid_[index];
	}

	[[nodiscard]] size_t NumUsed() const
	{
		size_t result = 0;

		for (size_t i = 0; i < size_; i++)
		{
			if (valid_[i])
				result++;
		}

		return result;
	}

	void SetStepSize(const size_t stepSize)
	{
		stepSize_ = stepSize;
	}

	void SetSize(const size_t size)
	{
		const auto oldSize = size_;

		if (size == oldSize)
			return;

		if (size == 0)
		{
			delete[] data_;
			delete[] valid_;
			data_ = nullptr;
			valid_ = nullptr;
			size_ = 0;
			return;
		}

		size_ = size;
		auto* const newData = new T[size];
		auto* const newValid = new bool[size];

		for (size_t i = 0; i < std::min(size, oldSize); i++)
		{
			newData[i] = data_[i];
			newValid[i] = valid_[i];
		}

		if (size > oldSize)
		{
			for (size_t i = oldSize; i < size; i++)
				newValid[i] = false;
		}

		delete[] data_;
		delete[] valid_;

		data_ = newData;
		valid_ = newValid;
	}

	void PutData(const T& value)
	{
		size_t index = 0;

		while (valid_[index])
		{
			if (index == size_)
			{
				SetSize(size_ + stepSize_);
				break;
			}

			index++;
		}

		data_[index] = value;
		valid_[index] = true;
	}

	void RemoveData(const size_t index) const
	{
		assert(index < size_);

		if (!valid_[index])
			std::print("DArray::RemoveData called with invalid index {}.\n", index);

		valid_[index] = false;
	}

	T GetData(size_t index) const
	{
		assert(index < size_);

		if (!valid_[index])
			std::print("DArray::GetData called with invalid index {}.\n", index);

		return data_[index];
	}

	void Empty()
	{
		delete[] data_;
		delete[] valid_;

		data_ = nullptr;
		valid_ = nullptr;
		size_ = 0;
	}

	T operator[](const size_t index) const
	{
		assert(index < size_);

		if (!valid_[index])
			std::print("DArray::[] called with invalid index {}.\n", index);

		return GetData(index);
	}
};
