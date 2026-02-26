#pragma once

template<typename T> struct LListItem
{
	T Data;
	LListItem* Next;
	LListItem* Prev;
};

template<typename T> class LList
{
	LListItem<T>* _first = nullptr;
	LListItem<T>* _last = nullptr;
	LListItem<T>* _cached = nullptr;
	int _cachedIndex = -1;
	int _size = 0;

public:
	~LList()
	{
		Empty();
	}

	void Empty()
	{
		auto item = _first;

		while (item)
		{
			const auto next = item->Next;
			delete item;
			item = next;
		}

		_first = nullptr;
		_last = nullptr;
		_size = 0;
		_cached = nullptr;
		_cachedIndex = -1;
	}

	[[nodiscard]] int Size() const
	{
		return _size;
	}

	[[nodiscard]] bool ValidIndex(const int index) const
	{
		return index >= 0 && index < _size;
	}

	void PutDataAtEnd(const T& data)
	{
		auto item = new LListItem<T>();
		item->Data = data;

		_size++;

		if (_last)
		{
			item->Prev = _last;
			item->Next = nullptr;
			_last->Next = item;
			_last = item;
		}
		else
		{
			item->Prev = nullptr;
			item->Next = nullptr;
			_first = item;
			_last = item;
		}

		_cachedIndex = -1;
		_cached = nullptr;
	}

	void PutDataAtStart(const T& data)
	{
		auto item = new LListItem<T>();
		item->Data = data;

		_size++;

		if (_last)
		{
			item->Prev = nullptr;
			item->Next = _first;
			_first->Prev = item;
			_first = item;
		}
		else
		{
			item->Prev = nullptr;
			item->Next = nullptr;
			_first = item;
			_last = item;
		}

		_cachedIndex = -1;
		_cached = nullptr;
	}

	void RemoveData(const int index)
	{
		auto i = 0;
		for (auto item = _first; item; item = item->Next)
		{
			if (i == index)
			{
				const auto prev = item->Prev;
				const auto next = item->Next;

				if (prev)
					prev->Next = next;
				else
					_first = next;

				if (next)
					next->Prev = item->Prev;
				else
					_last = prev;

				delete item;

				_size--;
				_cachedIndex = -1;
				_cached = nullptr;
				return;
			}

			i++;
		}
	}

	T GetData(const int index)
	{
		if (_cached && _cachedIndex != -1)
		{
			if (index == _cachedIndex + 1)
			{
				_cachedIndex++;
				_cached = _cached->Next;
				if (!_cached)
					return { };

				return _cached->Data;
			}

			if (index == _cachedIndex)
				return _cached->Data;
		}

		int i = 0;
		for (auto item = _first; item; item = item->Next)
		{
			if (i == index)
			{
				_cached = item;
				_cachedIndex = index;
				return item->Data;
			}

			i++;
		}

		_cachedIndex = -1;
		return { };
	}

	T operator[](const int index)
	{
		return GetData(index);
	}
};

// For testing
//template class LList<int>;
//template class LList<void*>;
