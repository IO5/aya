#pragma once

#include "GCManagedMemory.hpp"

namespace aya {

class GarbageCollector {
public:
    void registerObj(GCManagedMemory& object) {
        managedObjects.push_back(object);
    }
    void collect() {
        // TODO mark phase
        auto sweep = [](GCManagedMemory& obj) { return obj.sweep(); };
        // TODO if garbage is alive, then it's probably because of cycles,
        // we have to take spacial care no to invalidate any shared ptrs
        auto deleter = [](GCManagedMemory* obj) { /*TODO*/ };
        managedObjects.remove_and_dispose_if(sweep, deleter);
    }
private:
    GCManagedMemoryList managedObjects;
};

}