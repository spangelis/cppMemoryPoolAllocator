#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include "utils.h"

constexpr bool DEBUG = false;

bool compare(const std::pair<size_t, Pool>& lhs, const std::pair<size_t, Pool>& rhs);


template <class T>
class MemoryBlockAllocator {
private:
    size_t pools_amount_;

    size_t bin_search(std::vector<std::pair<size_t, Pool>>& arr, size_t x) {
        size_t size = arr.size();
        size_t left = 0;
        size_t right = size;
        while (left < right) {
            size_t mid = left + (right - left) / 2;
            if (x <= arr[mid].first) {
                right = mid;
            } else {
                left = mid + 1;
            }
        }
        return left;
    }

public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef size_t size_type;

    std::vector<std::pair<size_type, Pool>> pools_;
    std::map<pointer, int> position_to_allocate_pointer_;

    MemoryBlockAllocator() = default;

    template<typename T2>
    explicit MemoryBlockAllocator(const MemoryBlockAllocator<T2>& rhs) noexcept {
        pools_ = rhs.pools_;
        pools_amount_ = rhs.getPoolNumber();
    }

    explicit MemoryBlockAllocator(std::vector<std::pair<int, size_type>>& memory_to_allocate) {
        pools_amount_ = memory_to_allocate.size();
        for (size_t i = 0; i < pools_amount_; ++i) {
            int count_chunks = memory_to_allocate[i].first;
            size_t size_chunk = memory_to_allocate[i].second;
            pools_.emplace_back(size_chunk, Pool(count_chunks, size_chunk));
        }
        sort(pools_.begin(), pools_.end(), compare);
    }

    ~MemoryBlockAllocator() {
        pools_.clear();
    }

    pointer allocate(size_type chunksNum) {
        size_t paste_position = bin_search(pools_, chunksNum * sizeof(value_type));

        if (DEBUG) {
            std::cout << "Chunks num: " << chunksNum << ", weight = " << chunksNum * sizeof(value_type) << "\n";
        }

        for (size_t i = paste_position; i < pools_.size(); i++)
            if (pools_[i].second.havePlace()) {
                auto *ptr = (pointer) pools_[i].second.allocateInPool();
                position_to_allocate_pointer_[ptr] = i;
                return ptr;
            }
        throw std::bad_alloc();
    }

    void deallocate(pointer ptr, size_type elementsCount) {
        int dealloc_pos = position_to_allocate_pointer_[ptr];
        auto& group_to_deallocate = pools_[dealloc_pos];
        group_to_deallocate.second.deallocateInPool(ptr);
    }

    [[nodiscard]] size_type getPoolNumber() const {
        return pools_amount_;
    }
};

template <class T, class T2>
bool operator==(const MemoryBlockAllocator<T>& lhs, const MemoryBlockAllocator<T2>& rhs) {
    return (lhs.pools_ == rhs.pools_ && lhs.getPoolNumber() == rhs.getPoolNumber());
}

template <class T, class T2>
bool operator!=(const MemoryBlockAllocator<T>& lhs, const MemoryBlockAllocator<T2>& rhs) {
    return (lhs.pools_ != rhs.pools_ || lhs.getPoolNumber() != rhs.getPoolNumber());
}