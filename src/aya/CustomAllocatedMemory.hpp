#pragma once

#include <cassert>

namespace aya {

class Mallocator {
public:
    static void* alloc(size_t size) { return ::operator new(size); }
    static void  free(void* ptr, size_t size) { (void)size; ::operator delete(ptr); }

    template <typename T, typename... Args>
    static T* create(Args... args) {
        void* ptr = alloc(sizeof(T));
        return ::new (ptr) T(args...);
    }
    template <typename T>
    static void destroy(const T* ptr, size_t size = sizeof(T)) {
        ptr->~T();
        free((void*)ptr, size);
    }
};

using AlloctorType = Mallocator;

/// object that knows how to deallocate itself when reference count hits 0
/// IMPORTANT should be the first Base class when multiple deriving
class CustomAllocatedMemory {
public:
    // TODO
    CustomAllocatedMemory() : allocator(Mallocator()) {}
    virtual ~CustomAllocatedMemory() {
        assert(refCount == 0);
    }

    void* operator new     (size_t) = delete;
    void* operator new[]   (size_t) = delete;
    //void  operator delete  (void*) = delete;
    //void  operator delete[](void*) = delete;

    friend void intrusive_ptr_add_ref(CustomAllocatedMemory* p) noexcept { ++(p->refCount); }
    friend void intrusive_ptr_release(CustomAllocatedMemory* p) noexcept {
        if (--(p->refCount) == 0)
            destroy(p);
    }

protected:
    virtual size_t sizeOf() const = 0;

    mutable int refCount = 0;
    // TODO
    AlloctorType/*&*/ allocator;

private:
    /// destruct and deallocate
    static void destroy(const CustomAllocatedMemory* ptr) {
        auto& allocator = ptr->allocator;
        size_t size = ptr->sizeOf();
        allocator.destroy(ptr, size);
    }
};

}
