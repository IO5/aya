#pragma once

#include <boost/intrusive/list.hpp>

//TODO
#include <iostream>

namespace aya {

class Mallocator {
public:
    static void* alloc(size_t size) { return ::operator new(size); }
    static void  free(void* ptr, size_t size) { ::operator delete(ptr); }
};


template <class Allocator_>
class AllocatorProxy {
public:
    using Allocator = Allocator_;
#ifdef DUMMY
    AllocatorProxy(Allocator& alloc) : allocator(alloc) {}
    void* alloc(size_t size) const { return allocator.alloc(size); }
    void  free(void* ptr, size_t size) const { allocator.free(ptr, size); }
    Allocator& getAllocator() const { return allocator; }

private:
    Allocator& allocator;
#else
    void* alloc(size_t size) const { return ::operator new(size); }
    void  free(void* ptr, size_t size) const { (void)size; ::operator delete(ptr); }
    Allocator& getAllocator() const { return allocator; }

private:
    static Allocator allocator;
#endif
};

class CustomAllocatedMemory : private AllocatorProxy<Mallocator> {
public:
    using AllocatorProxy<Mallocator>::AllocatorProxy;
    virtual ~CustomAllocatedMemory() {
        lastAllocator = &getAllocator();
    }

    void* operator new[]   (size_t) = delete;
    void  operator delete[](void*)  = delete;

    void* operator new (size_t size) {
        return Allocator::alloc(size);
    }
    void operator delete (void* ptr, size_t size) {
        lastAllocator->free(ptr, size);
    }

private:
    static thread_local Allocator* lastAllocator;
};

using ListBaseHook =
    boost::intrusive::list_base_hook<boost::intrusive::link_mode<boost::intrusive::auto_unlink>>;

class GCManagedMemory: public ListBaseHook {
public:
    virtual ~GCManagedMemory() = default;

    void mark() const {
        // TODO change to iterative. Have to protect ourselves from malicious stack overflow
        if (!marked) {
            markImpl();
            marked = true;
        }
    }

    bool sweep() const {
        if (marked) {
            marked = false;
            return false;
        } else {
            // safeguard in case of a cycle, prevents from indirectly deleting itself
            ++refCount;
            clearAllRef();
            assert(refCount-- == 1);
            return true;
        }
    }

protected:
    GCManagedMemory() = default;

    virtual void markImpl() const {}
    virtual void clearAllRef() const {}

private:
    friend void intrusive_ptr_add_ref(const GCManagedMemory* p) noexcept { ++(p->refCount); }
    friend void intrusive_ptr_release(const GCManagedMemory* p) noexcept {
        if (--(p->refCount) == 0) {
            //TODO
            //destroy(p);
        }
    }
    mutable int refCount = 0;
    mutable bool marked = false;
};

using GCManagedMemoryList =
    boost::intrusive::list<GCManagedMemory, boost::intrusive::constant_time_size<false>>;

}

