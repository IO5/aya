#pragma once

#include <functional>

namespace aya {

class CFunction : public std::function<void(gsl::span<Value>&)> {
public:
    using std::function<void(gsl::span<Value>&)>::function;

    CFunction() = delete;
    ~CFunction() {
        assert(refCount == 0);
    }
private:
    friend void intrusive_ptr_add_ref(const CFunction* p) noexcept { ++(p->refCount); }
    friend void intrusive_ptr_release(const CFunction* p) noexcept {
        if (--(p->refCount) == 0)
            delete p;
    }
    mutable int refCount = 0;
};

}
