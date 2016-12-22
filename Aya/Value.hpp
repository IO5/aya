#pragma once

#include "Types.hpp"

#include <boost/smart_ptr/intrusive_ptr.hpp>

#include <gsl>
#include <variant>
#include <any>

namespace aya {

template<typename T, typename... Types>
using is_one_of = std::disjunction<std::is_same<T, Types>...>;
template<typename T, typename... Types>
constexpr bool is_one_of_v = is_one_of<T, Types...>::value;

template <typename T>
using IntrusivePtr = boost::intrusive_ptr<T>;
//template <typename T>
//void intrusive_ptr_add_ref(const T* ptr) noexcept;
//template <typename T>
//void intrusive_ptr_release(const T* ptr) noexcept;


class CFunction;
class Object;

using Nil = std::monostate;

inline string_t toString(Nil) { return "nil"; }
inline string_t toString(bool val) { return val ? "true" : "false"; }
inline string_t toString(int_t val) { return std::to_string(val); }
// TODO
inline string_t toString(real_t val) { return std::to_string(val); }
inline string_t toString(Object&) { return "todo"; }

inline string_t toString(CFunction&) { return "cfunc"; }


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
    using is_contained_type = is_one_of<T, Nil, bool, int_t, real_t, Object, CFunction>;
    template <typename T>
    static constexpr bool is_contained_type_v = is_contained_type::value;

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
    constexpr bool is() const {
        return std::holds_alternative<interface_to_inner_t<T>>(v);
    }

    template <typename... Types>
    constexpr bool isOneOf() const {
        static_assert(std::conjunction_v<is_contained_type<Types>...>, "Type not in the variant");
        return std::visit([](const auto& arg) {
            // I could remove const&, but I'm unsure if it gives the same semantics
            return is_one_of_v<
                inner_to_interface_t<
                    std::remove_const_t<
                        std::remove_reference_t<
                            decltype(arg)
                >>>, Types...>;
        }, v);
    }
    
    // TODO I'm not positive that we need const versions at all
    template <typename T>
    constexpr T& get() {
        return unpack(std::get<interface_to_inner_t<T>>(v));
    }
    template <typename T>
    constexpr const T& get() const {
        return unpack(std::get<interface_to_inner_t<T>>(v));
    }

    template <typename T>
    constexpr T* getIf() & {
        auto* ptr = std::get_if<interface_to_inner_t<T>>(&v);
        return ptr ? &unpack(*ptr) : nullptr;
    }
    template <typename T>
    constexpr const T* getIf() const & {
        auto* ptr = std::get_if<interface_to_inner_t<T>>(&v);
        return ptr ? &unpack(*ptr) : nullptr;
    }

    template <typename T>
    T* getIf() && = delete;
    template <typename T>
    const T* getIf() const && = delete;

    bool operator==(const Value& other) const { return v == other.v; }
    bool operator!=(const Value& other) const { return ! (*this == other); }

    template <class Visitor>
    constexpr auto visit(Visitor&& vis) {
        return std::visit(std::forward<Visitor>(vis), v);
    }

    template <class Visitor, class... Variants>
    friend constexpr auto visit(Visitor&& vis, Variants&&... vars) ->
        decltype(std::visit(std::forward<Visitor>(vis), vars.v...));

    string_t toString() const {
        return std::visit([](const auto& arg) { return aya::toString(unpack(arg)); }, v);
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
    template <typename T> struct interface_to_inner { using type = T; };
    template <>           struct interface_to_inner<CFunction> { using type = CFunctionPtr; };
    template <>           struct interface_to_inner<Object> { using type = ObjectPtr; };
    template <typename T> using  interface_to_inner_t = typename interface_to_inner<T>::type;

    template <typename T> struct inner_to_interface { using type = T; };
    template <typename T> struct inner_to_interface<NotNull<IntrusivePtr<T>>> { using type = T; };
    template <typename T> using  inner_to_interface_t = typename inner_to_interface<T>::type;

    static Object& unpack(const ObjectPtr& arg) { return *(arg.get().get()); }
    static CFunction& unpack(const CFunctionPtr& arg) { return *(arg.get().get()); }
    template <typename T> static T& unpack(T& arg) { return arg; }
};

template <class Visitor, class... Variants>
constexpr auto visit(Visitor&& vis, Variants&&... vars) {
    return std::visit(std::forward<Visitor>(vis), vars.v...);
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