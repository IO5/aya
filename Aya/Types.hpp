#pragma once

#include <gsl>
#include <string>
#include <variant>

namespace aya {

using int_t = intptr_t;
using real_t = double;
using char_t = char;
using string_view = gsl::basic_string_span<const char_t>;
using string_t = std::basic_string<char_t>;

using Nil = std::monostate;

}