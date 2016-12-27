#pragma once

#include "Value.hpp"
#include "GCManagedMemory.hpp"
#include "CustomAllocatedMemory.hpp"

#include <boost/container/flat_map.hpp>

namespace aya {

// TODO
using Map = boost::container::flat_map<string_t, Value, std::less<string_t>/*, Alloctor*/>;

class TypeObject;

void mark(const Value& val);

class Object : public CustomAllocatedMemory, public GCManagedMemory {
public:
    ~Object() {
        if (instanceVars) {

        }

    }
    virtual const string_view* getIfString() { return nullptr; }
    // getIfList
    // getIfDict

    const Value& get(const string_t& key);
    void set(const string_t& key, const Value& val);
    void set(const string_t& key, Value&& val);

protected:
    Object(TypeObject& definition);

    size_t sizeOf() const override { return sizeof(*this); }
    void markImpl() const override;
    void sweepImpl() const override {
        // safeguard in case of a cycle, prevents from indirectly deleting itself
        ++refCount;
        //TODO
        if (instanceVars)
            instanceVars->clear();
        assert(refCount-- == 1);
    }

    TypeObject& definition;
    // I expect instance variables to be rare, that's why I put unique_ptr here instead of having 
    // the Map directly; save few more bytes per object
    // TODO allocator, and deleter
    Map* instanceVars;
private:
};

inline void mark(const Value& val) {
    if (auto* obj = val.getIf<Object>()) {
        obj->mark();
    }
}

}
