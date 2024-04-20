#include "badalloc.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <cmath>
#include <set>
#include <unordered_map>

std::pair<size_t, size_t> testVector(size_t amount, size_t repeat = 5) {
    // Amount of chunks, size of chunk
    std::vector<std::pair<int, size_t>> pools;
    size_t counter = 0;
    int amountOfElements = 1;
    while (amountOfElements <= pow(2, log2(amount) + 1)) {
        pools.emplace_back(1, amountOfElements * sizeof(int));
        amountOfElements *= 2;
    }

    MemoryBlockAllocator<int>testAlloc(pools);

    size_t res1 = 0;
    size_t res2 = 0;

    for (int j = 0; j < repeat; j++) {
        std::vector<int, MemoryBlockAllocator<int>> v1(testAlloc);

        auto start1 = std::chrono::system_clock::now();

        for (int i = 0; i < amount; i++) {
            v1.push_back(i);
        }

        auto end1 = std::chrono::system_clock::now();


        std::vector<int> v2;
        auto start2 = std::chrono::system_clock::now();

        for (int i = 0; i < amount; i++) {
            v2.push_back(i);
        }

        auto end2 = std::chrono::system_clock::now();

        res1 += std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        res2 += std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count();
        v1.clear();
        v2.clear();
    }
    res1 = res1 / repeat;
    res2 = res2 / repeat;

    std::cout << "\nThe time with BestAlloc for " << amount << " elements " << res1 << " ms\n";
    std::cout << "\nThe time without BestAlloc for " << amount << " elements " << res2 << " ms\n";

    return std::make_pair(res1, res2);
}

std::pair<size_t, size_t> testList(size_t amount, size_t repeat = 5) {
    // Amount of chunks, size of chunk
    std::vector<std::pair<int, size_t>> pools;
    pools.emplace_back(amount, 6 * sizeof(int));

    MemoryBlockAllocator<int>testAlloc(pools);

    size_t res1 = 0;
    size_t res2 = 0;

    for (int j = 0; j < repeat; j++) {
        std::list<int, MemoryBlockAllocator<int>> v1(testAlloc);

        auto start1 = std::chrono::system_clock::now();

        for (int i = 0; i < amount; i++) {
            v1.push_back(i);
        }

        auto end1 = std::chrono::system_clock::now();


        std::list<int> v2;
        auto start2 = std::chrono::system_clock::now();

        for (int i = 0; i < amount; i++) {
            v2.push_back(i);
        }

        auto end2 = std::chrono::system_clock::now();

        res1 += std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        res2 += std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count();
        v1.clear();
        v2.clear();
    }
    res1 = res1 / repeat;
    res2 = res2 / repeat;

    std::cout << "\nThe time for std::list with BestAlloc for " << amount << " elements " << res1 << " ms\n";
    std::cout << "\nThe time for std::list without BestAlloc for " << amount << " elements " << res2 << " ms\n";
    return std::make_pair(res1, res2);
}


std::pair<int, int> testSet(size_t amount, size_t repeat = 5) {
    // Amount of chunks, size of chunk
    std::vector<std::pair<int, size_t>> pools;
    pools.emplace_back(amount, 40);

    auto * ptr = new MemoryBlockAllocator<int>(pools);
    MemoryBlockAllocator<int> &testAlloc = *ptr;

    size_t res1 = 0;
    size_t res2 = 0;

    for (int j = 0; j < repeat; j++) {
        std::set<int, std::less<int>, MemoryBlockAllocator<int>> v1(testAlloc);

        auto start1 = std::chrono::system_clock::now();

        for (int i = 0; i < amount; i++) {
            v1.insert(i);
        }

        auto end1 = std::chrono::system_clock::now();


        std::set<int> v2;
        auto start2 = std::chrono::system_clock::now();

        for (int i = 0; i < amount; i++) {
            v2.insert(i);
        }

        auto end2 = std::chrono::system_clock::now();

        res1 += std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        res2 += std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count();
        delete ptr;
        ptr = new MemoryBlockAllocator<int>(pools);
        testAlloc = *ptr;

    }
    res1 = res1 / repeat;
    res2 = res2 / repeat;

    std::cout << "\nThe time for std::set with BestAlloc for " << amount << " elements " << res1 << " ms\n";
    std::cout << "\nThe time for std::set without BestAlloc for " << amount << " elements " << res2 << " ms\n";
    return std::make_pair(res1, res2);
}


void testUnorderedMap(size_t amount, size_t repeat = 5) {
    // Amount of chunks, size of chunk
    std::vector<std::pair<int, size_t>> pools;
    pools.emplace_back(amount * 16, 1000);

    MemoryBlockAllocator<std::pair<const int, int>>testAlloc(pools);

    size_t res1 = 0;
    size_t res2 = 0;

    for (int j = 0; j < repeat; j++) {
        std::unordered_map<int, int, std::hash<int>, std::equal_to<>, MemoryBlockAllocator<std::pair<const int, int>>> v1(testAlloc);

        auto start1 = std::chrono::system_clock::now();

        for (int i = 0; i < amount; i++) {
            v1[i] = i;
        }

        auto end1 = std::chrono::system_clock::now();


        std::unordered_map<int, int> v2;
        auto start2 = std::chrono::system_clock::now();

        for (int i = 0; i < amount; i++) {
            v2[i] = i;
        }

        auto end2 = std::chrono::system_clock::now();

        res1 += std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        res2 += std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count();
        v1.clear();
        v2.clear();
    }
    res1 = res1 / repeat;
    res2 = res2 / repeat;

    std::cout << "\nThe time for std::unordered_map with BestAlloc for " << amount << " elements " << res1 << " ms\n";
    std::cout << "\nThe time for std::unordered_map without BestAlloc for " << amount << " elements " << res2 << " ms\n";
}



int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int a, b;

    std::vector<int> f;
    std::vector<int> s;
    for (int i = 52500; i <= 60000; i += 2500) {
        auto t = testList(i, 1);
        f.push_back(t.first);
        s.push_back(t.second);
    }

    for (int i = 0; i < f.size(); ++i) {
        std::cout << f[i] << ", ";
    }
    std::cout << "\n";
    for (int i = 0; i < s.size(); ++i) {
        std::cout << s[i] << ", ";
    }

    return 0;
}