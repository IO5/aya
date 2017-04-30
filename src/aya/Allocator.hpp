#pragma once

#include <cassert>

class AllocTemplate {
public:
    void* alloc(size_t size);
    void  free(void* ptr, size_t size);
    void* realloc(void* ptr, size_t oldSize, size_t newSize);
    bool  owns(void* ptr);
};

template <size_t zeroOrderSize>
class BuddyAllocator {
public:
    void* alloc(size_t size);
    void  free(void* ptr, size_t size);
    void* realloc(void* ptr, size_t oldSize, size_t newSize);
    bool  owns(void* ptr);

};

struct NullAllocator {
    void* alloc(size_t) { return nullptr; }
    void  free(void* ptr, size_t) { assert(ptr == nullptr); }
    bool  owns(void*) { return false; }
};

template <typename Primary, typename Fallback>
class FallbackAllocator : private Primary, private Fallback {
public:
    void* alloc(size_t size) {
        if (void* ptr = Primary::alloc(size)) {
            return ptr;
        } else {
            return Fallback::alloc(size);
        }
    }

    void free(void* ptr, size_t size) {
        if (Primary::owns(ptr)) {
            Primary::free(ptr);
        } else {
            Fallback::free(ptr);
        }
    }

    void* realloc(void* ptr, size_t oldSize, size_t newSize);

    bool owns(void* ptr) { return Primary::owns(ptr) || Fallback::owns(ptr); }
};

#include <type_traits>
#include <memory>

template <size_t storageSize>
class StackAllocator {
public:
    void* alloc(size_t size) {
        size_t alignedSize = roundToAlign(size);
        if (top + alignedSize > end())
            return nullptr;
        void* ptr = top;
        top += size;
        return ptr;
    }

    void  free(void* ptr, size_t size) {
        if (ptr + roundToAlign(size) == top) {
            top = reinterpret_cast<uint8_t*>(ptr);
        }
    }
    void* realloc(void* ptr, size_t oldSize, size_t newSize);
    bool  owns(void* ptr) {
        return ptr >= begin() && ptr < end();
    }
private:
    size_t roundToAlign(size_t size) {
        constexpr Max_Align = std::alignment_of_v<double>;
        return size & ~(Max_Align - 1) + Max_Align;
    }

    std::aligned_storage_t<storageSize> storage;
    void* begin() { return &storage; }
    void* end() { return reinterpret_cast<uint8_t*>(&storage) + storageSize; }
    uint8_t* top = &storage;
};

//template <typename Derived, typename Allocator>
struct CustomAllocatedMemory {
    void* operator new(size_t) = delete;
    void* operator new[](size_t) = delete;
    void  operator delete(void*) = delete;
    void  operator delete[](void*) = delete;

    //void* operator new(size_t size, Allocator& allocator) {
    //    return allocator.alloc(size);
    //}
    //void  operator delete(void* ptr, Allocator& allocator) {
    //    allocator.free(ptr, sizeof(Derived));
    //}
};
