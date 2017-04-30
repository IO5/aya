#pragma once

#include "Types.hpp"
#include "Value.hpp"

namespace aya {

class CFunction;
class Object;

inline string_t toString(Nil) { return "nil"; }
inline string_t toString(bool val) { return val ? "true" : "false"; }
inline string_t toString(int_t val) { return std::to_string(val); }
// TODO
inline string_t toString(real_t val) { return std::to_string(val); }
inline string_t toString(const Object&) { return "TODO obj"; }
inline string_t toString(const CFunction&) { return "TODO cfunc"; }

inline string_t toString(const Value& val) {
    return val.visit([](auto& v) { return toString(v); });
}

template <typename T> string_t typeToString();
template <> inline string_t typeToString<Nil>() { return "nil"; }
template <> inline string_t typeToString<bool>() { return "bool"; }
template <> inline string_t typeToString<int_t>() { return "integer"; }
template <> inline string_t typeToString<real_t>() { return "real"; }
// TODO
template <> inline string_t typeToString<Object>() { return "TODO object"; }
template <> inline string_t typeToString<CFunction>() { return "TODO cfunc"; }

inline string_t typeToString(const Value& val) {
    return val.visit([](const auto& v) {
        return typeToString<std::decay_t<decltype(v)>>();
    });
}

}
