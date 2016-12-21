#pragma once
namespace aya {

class ManagedMemory {
public:
    virtual ~ManagedMemory() = default;

    void mark() {
        if (!marked) {
            markImpl();
            marked = true;
        }
    }

    bool sweep() {
        if (marked) {
            marked = false;
            return false;
        } else {
            return true;
        }
    }

    ManagedMemory(const ManagedMemory&) = delete;
    void* operator new(size_t) = delete;
    void* operator new[](size_t) = delete;
    //void  operator delete(void*) = delete;
    //void  operator delete[](void*) = delete;

protected:
    ManagedMemory() = default;

    virtual void markImpl() = 0;

    void* operator new(size_t s, void* ptr){
        return ptr;
    }

private:
    mutable bool marked = false;
};

}

