#pragma once

#include <cassert>
#include <print>

template<typename T> class DArray
{
	int _stepSize = 1;
	int _size = 0;
	T* _data = nullptr;
	bool* _valid = nullptr;

public:
	~DArray()
	{
		Empty();
	}

	[[nodiscard]] int Size() const { return _size; }

	void SetSize(const int size)
	{
		if (size <= 0)
		{
			_size = 0;
			delete[] _data;
			delete[] _valid;
			_data = nullptr;
			_valid = nullptr;
			return;
		}

		const auto oldSize = _size;

		if (size == oldSize)
			return;

		_size = size;

		const auto newData = new T[size];
		const auto newValid = new bool[_size];

		const auto copySize = size > oldSize ? oldSize : _size;

		for (auto i = 0; i < copySize; i++)
		{
			newData[i] = _data[i];
			newValid[i] = _valid[i];
		}

		for (auto i = oldSize; i < size; i++)
			newValid[i] = false;

		delete[] _data;
		delete[] _valid;
		_valid = newValid;
		_data = newData;
	}

	void SetStepSize(const int size) { _stepSize = size; }

	void Empty()
	{
		delete[] _data;
		delete[] _valid;
		_data = nullptr;
		_valid = nullptr;
		_size = 0;
	}

	[[nodiscard]] bool ValidIndex(const int index) const { return index < _size && index >= 0 && _valid[index]; }

	void PutData(const T& data)
	{
		for (auto i = 0; i < _size; i++)
		{
			if (_valid[i])
				continue;

			// NOTE: This matches the original code, but seems odd.
			if (i == -1)
				break;

			_data[i] = data;
			_valid[i] = true;
			return;
		}

		const auto index = _size;
		SetSize(_size + _stepSize);

		_data[index] = data;
		_valid[index] = true;
	}

	void PutData(const T& data, const int index)
	{
		assert(index < _size && index >= 0);
		_data[index] = data;
		_valid[index] = true;
	}

	void ChangeData(const T& data, const int index)
	{
		assert(index < _size && index >= 0);
		if (!_valid[index])
			std::println("Warning : DArray::ChangeData called, referenced unused data.  (Index ={})", index);
		PutData(data, index);
		_valid[index] = true;
	}

	void RemoveData(int index) const
	{
		assert(index < _size && index >= 0);
		if (!_valid[index])
			std::println("Warning : DArray::RemoveData called, referenced unused data.  (Index = {})", index);
		_valid[index] = false;
	}

	T& GetData(int index)
	{
		assert(index < _size && index >= 0);
		if (!_valid[index])
			std::println("DArray::GetData called, referenced unused data.  (Index = {})", index);
		return _data[index];
	}

	[[nodiscard]] int NumUsed() const
	{
		auto count = 0;
		for (auto i = 0; i < _size; i++)
		{
			if (_valid[i])
				count++;
		}
		return count;
	}

	T& operator[](const int index)
	{
		assert(index < _size && index >= 0);
		if (!_valid[index])
			std::println("DArray error : DArray::[] called, referenced unused data.  (Index = {})", index);
		return GetData(index);
	}
};

// For testing
//template class DArray<int>;
//template class DArray<void*>;
