#pragma once
#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <cmath>


class AllocParams {
private:
   size_t poolSize;
   size_t poolAmount;
   size_t chunkSize;
   std::vector<void *> pools;
   std::vector<size_t> poolCapacity;


   class Chunk {
   public:
       void* begin = nullptr;
       bool isEmpty = true;

       Chunk() = default;

       explicit Chunk(void * ptr, size_t step, size_t bytes) {
           char * t = static_cast<char*>(ptr);
           t += step * bytes;
           begin = t;
       }
   };

   std::vector<std::vector<AllocParams::Chunk>> chunks;


   template<class U>
   friend class BestAllocator;



public:
   AllocParams() : poolSize(16384), chunkSize(128), poolAmount(1) {
       init();
   };

   AllocParams(size_t poolSize, size_t chunkSize, size_t poolAmount) : poolSize(poolSize), chunkSize(chunkSize), poolAmount(poolAmount) {
       init();
   }

   ~AllocParams() {
       for (auto& pool : pools) {
           ::operator delete(pool);
       }
   }

private:
   void init() {
       assert(poolSize % chunkSize == 0 && "Размер пула должен быть кратен размеру кусочков");
       poolCapacity.assign(poolAmount, poolSize);
       chunks.resize(poolAmount);
       pools.resize(poolAmount);
       for (size_t i = 0; i < poolAmount; ++i) {
           chunks[i].resize(poolSize / chunkSize);
           auto pool = ::operator new(poolSize);
           pools[i] = pool;
           for (size_t j = 0; j < poolSize / chunkSize; j++) {
               chunks[i][j] = Chunk(pool, j, chunkSize);
           }
       }
   };
};


template<typename T>
class BestAllocator {
private:
   AllocParams * params;

public:
   using value_type = T;

   AllocParams * getParams() const {
       return params;
   }

   BestAllocator() = default;

   explicit BestAllocator(AllocParams * params) : params(params) {};

   template<class U>
   explicit BestAllocator(BestAllocator<U> const &allocator) noexcept {
       params = allocator.getParams();
   };


   T* allocate(size_t n) {
       assert(params->chunkSize >= sizeof(T) && "Размер кусочка должен быть больше размера типа");

       if ((n * sizeof(T) > params->poolSize) || *std::max_element(params->poolCapacity.begin(), params->poolCapacity.end()) < (n * sizeof(T))) {
           throw std::bad_alloc();
       }

       size_t minChunksAmount = ceil(double(n) * double(sizeof(T)) / double((params->chunkSize) - n * sizeof(T) % params->chunkSize));
       for (size_t pool = 0; pool < params->poolAmount; ++pool) {
           for (int j = 0; j < params->chunks[pool].size() - minChunksAmount + 1; j++) {
               bool flag = true;
               for (int k = 0; k < minChunksAmount; k++) {
                   if (!params->chunks[pool][j + k].isEmpty) {
                       flag = false;
                       break;
                   }
               }
               if (flag) {
                   for (int i = 0; i < minChunksAmount; ++i) {
                       params->chunks[pool][j + i].isEmpty = false;
                   }
                   params->poolCapacity[pool] -= minChunksAmount * params->chunkSize;
                   return static_cast<T*>(params->chunks[pool][j].begin);
               }
           }
       }

       throw std::bad_alloc();
   }

   void deallocate(T* p, size_t n) {
       for (size_t pool = 0; pool < params->poolAmount; ++pool) {
           size_t q = 0;
           for (auto iter = static_cast<T*>(params->chunks[pool][0].begin); iter < static_cast<T*>(params->chunks[pool][0].begin) + params->poolSize / sizeof(T); ++iter) {
               ++q;
               if (iter == p) {
                   params->chunks[pool][q * sizeof(T) / params->chunkSize].isEmpty = true;
                   params->poolCapacity[pool] += params->chunkSize;
               }
           }
       }
   }

   template<class ... Args>
   void construct(T* p, Args&& ... args) {
       new (p) T(std::forward<Args>(args) ...);
   }

   void destroy(T* p) {
       p->~T();
   }
};

template<class T, class U>
bool operator==(const BestAllocator<T>& a, const BestAllocator<U>& b) {
   if (a.getParams() == b.getParams()) {
       return true;
   }
   return false;
}

template<class T, class U>
bool operator!=(const BestAllocator<T>& a, const BestAllocator<U>& b) {
   if (a.getParams() != b.getParams()) {
       return true;
   }

   return false;
}