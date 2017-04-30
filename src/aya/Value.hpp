#pragma once

#include "Types.hpp"
#include "Utils.hpp"

#include <variant>
#include <type_traits>

namespace aya {

class Value;
string_t toString(const Value&);
string_t typeToString(const Value&);

class CFunction;
class Object;

class Value {
private:
    std::variant<
        Nil,
        bool,
        int_t,
        real_t,
        not_null<IntrusivePtr<Object>>,
        not_null<CFunction*>
    > v;

    using CFunctionPtr = not_null<CFunction*>;
    using ObjectPtr = not_null<IntrusivePtr<Object>>;

    template <typename T>
    using is_contained_type = is_one_of<T, Nil, bool, int_t, real_t, Object, CFunction>;
    template <typename T>
    static constexpr bool is_contained_type_v = is_contained_type<T>::value;

public:
    constexpr Value() {}
    constexpr Value(Nil) {}
    constexpr Value(bool val)                   : v(val) {}
    constexpr Value(int_t val)                  : v(val) {}
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    constexpr Value(T val)                      : v(int_t(val)) {}
    constexpr Value(real_t val)                 : v(val) {}

    // all that boilerplate is bool's fault
    Value(not_null<IntrusivePtr<Object>> val)   : v(val) {}
    Value(not_null<Object*> val)                : v(ObjectPtr(val)) {}
    Value(IntrusivePtr<Object> val)             : v(ObjectPtr(val)) {}
    Value(Object* val)                          : v(ObjectPtr(val)) {}
    Value(Object& val)                          : v(ObjectPtr(&val)) {}

    Value(not_null<CFunction*> val)             : v(val) {}
    Value(CFunction* val)                       : v(CFunctionPtr(val)) {}
    Value(CFunction& val)                       : v(CFunctionPtr(&val)) {}

    template <typename T>
    constexpr bool is() const {
        static_assert(is_contained_type_v<T>, "No such type in variant");
        return std::holds_alternative<interface_to_inner_t<T>>(v);
    }

    template <typename... Types>
    constexpr bool isOneOf() const {
        static_assert(std::conjunction_v<is_contained_type<Types>...>, "No such type in variant");
        return std::visit([](const auto& arg) {
            return is_one_of_v<
                inner_to_interface_t<
                    std::decay_t<
                        decltype(arg)
                >>, Types...>;
        }, v);
    }

    template <typename T>
    constexpr T& get() {
        static_assert(is_contained_type_v<T>, "No such type in variant");
        return unpack(std::get<interface_to_inner_t<T>>(v));
    }
    template <typename T>
    constexpr const T& get() const {
        static_assert(is_contained_type_v<T>, "No such type in variant");
        return unpack(std::get<interface_to_inner_t<T>>(v));
    }

    template <typename T>
    constexpr T* getIf() & {
        static_assert(is_contained_type_v<T>, "No such type in variant");
        auto* ptr = std::get_if<interface_to_inner_t<T>>(&v);
        return ptr ? &unpack(*ptr) : nullptr;
    }
    template <typename T>
    constexpr const T* getIf() const & {
        static_assert(is_contained_type_v<T>, "No such type in variant");
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
        auto visitor = [v = std::forward<Visitor>(vis)](auto&& arg) {
            return v(unpack(arg));
        };
        return std::visit(visitor, v);
    }
    template <class Visitor>
    constexpr auto visit(Visitor&& vis) const {
        return std::visit([visitor = std::forward<Visitor>(vis)](auto&& arg) {
            return visitor(unpack(arg));
        }, v);
    }

    string_t toString() const {
        return aya::toString(*this);
    }
    string_t typeToString() const {
        return aya::typeToString(*this);
    }

private:
    template <typename T> struct interface_to_inner { using type = T; };
    template <>           struct interface_to_inner<Object> { using type = ObjectPtr; };
    template <>           struct interface_to_inner<CFunction> { using type = CFunctionPtr; };
    template <typename T> using  interface_to_inner_t = typename interface_to_inner<T>::type;

    template <typename T> struct inner_to_interface { using type = T; };
    template <>           struct inner_to_interface<ObjectPtr> { using type = Object; };
    template <>           struct inner_to_interface<CFunctionPtr> { using type = CFunction; };
    template <typename T> using  inner_to_interface_t = typename inner_to_interface<T>::type;

    static Object& unpack(const ObjectPtr& arg) { return *(arg.get().get()); }
    static CFunction& unpack(const CFunctionPtr& arg) { return *(arg.get()); }
    template <typename T> static T& unpack(T& arg) { return arg; }
};

}
