#pragma once

namespace aya {

class Object {
public:
private:
    friend void intrusive_ptr_add_ref(const Object* p) noexcept { ++(p->refCount); }
    friend void intrusive_ptr_release(const Object* p) noexcept {
        if (--(p->refCount) == 0)
            delete p;
    }
    mutable int refCount = 0;
};

}
