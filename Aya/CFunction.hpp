#pragma once

#include "Value.hpp"

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

template <typename... Types>
struct UnpackStackImpl;

template <typename Head, typename... Tail>
struct UnpackStackImpl<Head, Tail...> {
    static auto unpackStack(const gsl::span<Value>& stack) {
        if (Head* val = stack[0].getIf<Head>()) {
            return std::tuple_cat(
                std::tuple<Head>(*val),
                UnpackStackImpl<Tail...>::unpackStack(stack.subspan(1, stack.size() - 1))
            );
        } else {
            throw 1;
        }
    }
};

namespace detail {

template <typename... Types>
struct UnpackStackImpl;

template <typename Head, typename... Tail>
struct UnpackStackImpl<Head, Tail...> {
    static auto unpackStack(const gsl::span<Value>& stack) {
        if (Head* val = stack[0].getIf<Head>()) {
            return std::tuple_cat(
                std::tuple<Head>(*val),
                UnpackStackImpl<Tail...>::unpackStack(stack.subspan(1, stack.size() - 1))
            );
        } else {
            throw 1;
        }
    }
};

template <>
struct UnpackStackImpl<> {
    static auto unpackStack(const gsl::span<Value>&) {
        return std::tuple<>();
    }
};

template <typename... Types>
auto unpackStack(const gsl::span<Value>& stack) {
    return UnpackStackImpl<Types...>::unpackStack(stack);
}

}

template <typename Ret, typename... Args, typename Func>
NotNull<CFunction*> makeBind(Func&& func) {
    return new CFunction([f = std::forward<Func>(func)](gsl::span<Value>& stack) {
        assert(stack.size() == sizeof...(Args));
        auto args = detail::unpackStack<Args...>(stack);
        //stack[0].get
        try {
            std::apply(f, args);
        } catch (...) {

        }
        return 1;
    });
}

}
