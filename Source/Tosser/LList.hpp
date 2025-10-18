#pragma once

#include <cstdint>
#include <limits>

template<typename T>
struct LListItem
{
    T data;
    LListItem* next = nullptr;
    LListItem* prev = nullptr;
};

template<typename T>
class LList
{
    static constexpr size_t CACHED_INDEX_INVALID = std::numeric_limits<size_t>::max();

    struct LListItem<T>* first_ = nullptr;
    struct LListItem<T>* last_ = nullptr;
    struct LListItem<T>* cached_ = nullptr;
    size_t cachedIndex_ = CACHED_INDEX_INVALID;
    size_t size_ = 0;

public:
    LList() {}
    ~LList()
    {
        Empty();
    }

    size_t Size() const
    {
        return size_;
    }

    bool ValidIndex(size_t index) const
    {
        return index < size_;
    }

    void PutData(const T& data)
    {
        PutDataAtEnd(data);
    }

    void PutDataAtStart(const T& data)
    {
        auto* const newItem = new LListItem<T>;
        newItem->data = data;
        size_++;

        if (!first_)
        {
            assert(!last_); // Sanity check
            first_ = newItem;
            last_ = newItem;
        }
        else
        {
            first_->prev = newItem;
            newItem->next = first_;
            first_ = newItem;
        }
    }

    void PutDataAtEnd(const T& data)
    {
        auto* const newItem = new LListItem<T>();
        newItem->data = data;
        size_++;

        if (!last_)
        {
            assert(!first_); // Sanity check
            first_ = newItem;
            last_ = newItem;
        }
        else
        {
            last_->next = newItem;
            newItem->prev = last_;
            last_ = newItem;
        }
    }
    void RemoveData(size_t index)
    {
        auto* item = first_;

        for (size_t i = 0; i < index; i++)
        {
            if (!item)
                return;

            item = item->next;
        }

        if (!item)
            return;

        if (!item->prev)
            first_ = item->next;

        if (!item->next)
            last_ = item->prev;

        if (item->prev)
            item->prev->next = item->next;

        if (item->next)
            item->next->prev = item->prev;

        delete item;

        size_--;
        cachedIndex_ = CACHED_INDEX_INVALID;
        cached_ = nullptr;
    }

    T GetData(size_t index)
    {

        if (cached_ && cachedIndex_ != CACHED_INDEX_INVALID)
        {
            if (cachedIndex_ + 1 == index)
            {
                const auto resultItem = cached_->next;
                cachedIndex_ = index;
                cached_ = resultItem;

                if (!resultItem)
                    return 0;

                return resultItem->data;
            }

            if (index == cachedIndex_)
                return cached_->data;
        }

        auto* current = first_;


        for (size_t i = 0; i < index; i++)
        {
            if (!current)
                return 0;

            current = current->next;
        }

        cached_ = current;

        if (!current)
        {
            cachedIndex_ = CACHED_INDEX_INVALID;
            return current;
        }

        cachedIndex_ = index;
        return current->data;
    }

    void Empty()
    {
        auto* current = first_;
        while (current)
        {
            auto* const next = current->next;
            delete current;
            current = next;
        }

        first_ = nullptr;
        last_ = nullptr;
        size_ = 0;
        cached_ = nullptr;
        cachedIndex_ = CACHED_INDEX_INVALID;
    }

    T operator[](size_t index)
    {
        return GetData(index);
    }
};
