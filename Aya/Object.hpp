#pragma once

#include "Value.hpp"
#include "GCManagedMemory.hpp"

#include <boost/container/flat_map.hpp>

namespace aya {

using Map = boost::container::flat_map<string_t, Value, std::less<string_t>/*, Alloctor*/>;

class TypeObject;

void mark(const Value& val);

class Object : public GCManagedMemory {
public:
    const Value& get(string_view key);
    void set(string_view key, Value val);

protected:
    Object();
    Object(TypeObject& definition);

    void markImpl() const override;
    void clearAllRef() const override {
        //TODO
        //definition.reset();
        if (instanceVars)
            instanceVars->clear();
    }

    IntrusivePtr<TypeObject> definition;
    // I expect instance variables to be rare, that's why I put unique_ptr here instead of having 
    // the Map directly; save few more bytes per object
    // TODO allocator, and deleter
    std::unique_ptr<Map> instanceVars;
private:
};

inline void mark(const Value& val) {
    if (auto* obj = val.getIf<Object>()) {
        obj->mark();
    }
}

}
