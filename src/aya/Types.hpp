#pragma once

#include <string>
#include <string_view>
#include <variant>

namespace aya {

using int_t = int64_t;
using real_t = double;
using char_t = char;
using string_t = std::basic_string<char_t>;
using string_view = std::basic_string_view<char_t>;

using Nil = std::monostate;

}