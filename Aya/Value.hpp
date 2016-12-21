#pragma once

#include "Types.hpp"

#include <boost/smart_ptr/intrusive_ptr.hpp>

#include <gsl>
#include <variant>
#include <any>

namespace aya {

template<typename T, typename... Types>
constexpr bool is_one_of_v = std::disjunction_v<std::is_same<T, Types>...>;

template <typename T>
using IntrusivePtr = boost::intrusive_ptr<T>;
template <typename T>
void intrusive_ptr_add_ref(const T* ptr) noexcept;
template <typename T>
void intrusive_ptr_release(const T* ptr) noexcept;

class CFunction;
class Object;

class Value;
string_t toString(const Value& val);

using Nil = std::monostate;

template <typename... Types>
struct ValueBase {

};

class Value {
private:
    std::variant<
        Nil,
        bool,
        int_t,
        real_t,
        NotNull<IntrusivePtr<Object>>,
        NotNull<IntrusivePtr<CFunction>>
    > v;

    using CFunctionPtr = NotNull<IntrusivePtr<CFunction>>;
    using ObjectPtr = NotNull<IntrusivePtr<Object>>;

    template <typename T>
    struct interface_to_inner { using type = T; };
    template <typename T>
    struct interface_to_inner<NotNull<IntrusivePtr<T>>> { using type = T; };
    template <typename T>
    using interface_to_inner_t = typename interface_to_inner<T>::type;

    template <typename T>
    struct inner_to_interface { using type = T; };
    template <>
    struct inner_to_interface<CFunction> { using type = NotNull<IntrusivePtr<CFunction>>; };
    template <>
    struct inner_to_interface<Object> { using type = NotNull<IntrusivePtr<Object>>; };
    template <typename T>
    using inner_to_interface_t = typename inner_to_interface<T>::type;
public:
    constexpr Value() {}
    constexpr Value(Nil) {}
    constexpr Value(bool val)                   : v(val) {} 
    constexpr Value(int_t val)                  : v(val) {}
    constexpr Value(real_t val)                 : v(val) {}

    // all that boilerplate is bool's fault
    Value(NotNull<IntrusivePtr<Object>> val)    : v(val) {}
    Value(NotNull<Object*> val)                 : v(ObjectPtr(val)) {}
    Value(IntrusivePtr<Object> val)             : v(ObjectPtr(val)) {}
    Value(Object* val)                          : v(ObjectPtr(val)) {}
    Value(Object& val)                          : v(ObjectPtr(&val)) {}

    Value(NotNull<IntrusivePtr<CFunction>> val) : v(val) {}
    Value(NotNull<CFunction*> val)              : v(CFunctionPtr(val)) {}
    Value(IntrusivePtr<CFunction> val)          : v(CFunctionPtr(val)) {}
    Value(CFunction* val)                       : v(CFunctionPtr(val)) {}
    Value(CFunction& val)                       : v(CFunctionPtr(&val)) {}

    template <typename T>
    bool is() const { return std::holds_alternative<interface_to_inner_t<T>>(v); }

    template <typename... Types>
    bool isOneOf() const {
        return std::visit([](auto arg) {
            return is_one_of_v<interface_to_inner_t<decltype(arg)>, Types...>;
        }, v);
    }

    //bool isNil()       const { return is<Nil>(); }
    //bool isBool()      const { return is<bool>(); }
    //bool isInt()       const { return is<int_t>(); }
    //bool isReal()      const { return is<real_t>(); }
    //bool isObject()    const { return is<Object>(); }
    //bool isCFunction() const { return is<CFunction>(); }

    template <typename T>
    T& get() { return std::get<interface_to_inner_t<T>>(v); }
    template <typename T>
    const T& get() const { return std::get<interface_to_inner_t<T>>(v); }

    template <typename T>
    T* get_if() & { return std::get_if<interface_to_inner_t<T>>(&v); }
    template <typename T>
    const T* get_if() const & { return std::get_if<interface_to_inner_t<T>>(&v); }

    //bool operator==(const Value& other) const { return v == other.v; }
    //bool operator!=(const Value& other) const { return v != other.v; }

    template <class Visitor>
    constexpr auto visit(Visitor&& vis) {
        return std::visit(std::forward<Visitor>(vis), v);
    }

    template <class Visitor, class... Variants>
    friend constexpr auto visit(Visitor&& vis, Variants&&... vars) ->
        decltype(std::visit(std::forward<Visitor>(vis), vars.v...));

    string_t toString() const {
        return aya::toString(*this);
    }

    //string_t typeToString() {
    //    return ;
    //}

    //template <typename... Types>
    //std::variant<Types...> getSubvariant() {
    //    return std::visit([](auto&& val) {
    //        std::variant<Types...> result;
    //        if constexpr (is one of Types)
    //          result = val;
    //        return result
    //    }, v);
    //}
private:

};

template <class Visitor, class... Variants>
constexpr auto visit(Visitor&& vis, Variants&&... vars) {
    return std::visit(std::forward<Visitor>(vis), vars.v...);
}

string_t toString(Nil) { return "nil"; }
string_t toString(bool val) { return val ? "true" : "false"; }
string_t toString(int_t val) { return std::to_string(val); }
// TODO
string_t toString(real_t val) { return std::to_string(val); }
string_t toString(Object&) { return "todo"; }

string_t toString(CFunction&) { return "cfunc"; }
//string_t toString(void* val) { return std::to_string(reinterpret_cast<intptr_t>(val)); }

#include <boost/hana/functional/overload.hpp>

string_t toString(const Value& val) {
    return "";
    //auto visitor = boost::hana::overload(
    //    [](CFunctionPtr ptr) { return aya::toString(ptr.get().get()); },
    //    [](auto arg) { return aya::toString(arg); }
    //);
    //return visit(visitor, val);
}

//template <typename T> string_t typeToString();
//
//template <> string_t typeToString<Nil>() { return "nil"; }
//template <> string_t typeToString<bool>() { return "bool"; }
//template <> string_t typeToString<int_t>() { return "integer"; }
//template <> string_t typeToString<real_t>() { return "real"; }
//template <> string_t typeToString<NotNull<Object*>>() { return "ref"; }
//
//string_t typeToString(const Value& val) {
//    return visit([](auto&& arg) { return typeToString<decltype(arg)>(); }, val);
//}

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