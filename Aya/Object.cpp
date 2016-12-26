#include "Object.hpp"
// TODO wat
#include "CFunction.hpp"

namespace aya {

class TypeObject : public Object {};

void Object::markImpl() const {
    assert(definition);
    definition->mark();
    if (instanceVars) {
        for (auto& var : *instanceVars)
            aya::mark(var.second);
    }
    //TODO
}

}