#include <cstdlib>
#include <utility>

template <class T>
class free_chunk_manager {
private:
    T* container_;
    bool* free_chunks_;
    size_t size_;
    size_t count_free_chunks_;
    size_t index_for_fill_;
    size_t index_for_allocate_;
public:
    free_chunk_manager() {
        size_ = 0;
        count_free_chunks_ = 0;
        index_for_fill_ = 0;
        index_for_allocate_ = 0;
        container_ = nullptr;
        free_chunks_ = nullptr;
    }

    void resize(size_t elementCount) {
        delete[] container_;
        delete[] free_chunks_;
        free_chunks_ = new bool[elementCount];
        container_ = new T[elementCount];

        size_ = elementCount;
        count_free_chunks_ = elementCount;
        index_for_allocate_ = 0;
        index_for_fill_ = 0;

        for(size_t i = 0; i < elementCount; i++) {
            free_chunks_[i] = true;
        }
    }

    void fill(T element_for_fill) {
        container_[index_for_fill_++] = element_for_fill;
    }

    void pushFreeChunk(T elementForPush) {
        for (int i = 0; i < size_ ; i++) {
            if (container_[i] == elementForPush) {
                free_chunks_[i] = true;
                count_free_chunks_++;
                return;
            }
        }
    }

    T GetFreeChunk() {
        if (index_for_allocate_ >= size_ - 1) {
            index_for_allocate_ = 0;
        }

        for (size_t i = index_for_allocate_; i < size_; i++) {

            if (free_chunks_[i]) {
                free_chunks_[i] = false;
                --count_free_chunks_;
                index_for_allocate_ = i;
                return container_[i];
            }
        }
        return nullptr;
    }

    [[nodiscard]] size_t GetCountFreeChunks() const {
        return count_free_chunks_;
    }

    [[nodiscard]] size_t GetSize() const {
        return size_;
    }
};


class Pool {
private:
    size_t used_chunks_count_;
    size_t chunks_count_;
    size_t size_;
    free_chunk_manager<void*> free_chunks_container_;

public:
    Pool(): used_chunks_count_(0), chunks_count_(0), size_(0) {}

    Pool(size_t chunksCount, size_t size): chunks_count_(chunksCount), size_(size), used_chunks_count_(0) {

        free_chunks_container_.resize(chunks_count_);
        auto* chunk = (char*)(malloc(chunks_count_ * size_));

        for (size_t i = 0; i < chunks_count_; ++i) {
            free_chunks_container_.fill(chunk);
            if (i != chunks_count_ - 1) {
                chunk += size_;
            }
        }
    }

    [[nodiscard]] bool havePlace() const {
        return (used_chunks_count_ < chunks_count_ && free_chunks_container_.GetCountFreeChunks() > 0);
    }

    void* getFreeChunk() {
        auto *res = free_chunks_container_.GetFreeChunk();
        return res;
    }

    void* allocateInPool() {
        ++used_chunks_count_;
        return getFreeChunk();
    }

    void deallocateInPool(void* chunk) {
        --used_chunks_count_;
        free_chunks_container_.pushFreeChunk(chunk);
    }

    [[nodiscard]] size_t GetChunkSize() const {
        return size_;
    }

    [[nodiscard]] size_t GetChunksCount() const {
        return chunks_count_;
    }
};