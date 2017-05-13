#include "Token.hpp"

#include <iostream>
#include <boost/hana/type.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/string.hpp>
#include <boost/hana/adjust_if.hpp>
#include <boost/hana/front.hpp>
#include <boost/hana/equal.hpp>
#include <string_view>
#include <type_traits>

namespace aya {

namespace detail {

namespace bh = boost::hana;

template <char_t ch>
using char_ = bh::integral_constant<char_t, ch>;
template <char_t ch>
constexpr char_<ch> char_c{};

template <auto result>
using result_ = bh::integral_constant<decltype(result), result>;
template <auto result>
constexpr result_<result> result_c{};

template <char_t Ch, auto Result, typename... Children>
struct Node {
    static constexpr char_t ch = Ch;
    static constexpr auto result = Result;
    static constexpr bh::tuple<Children...> children{};

    template <typename T>
    constexpr bool operator==(const T&) const { return std::is_same_v<Node, T>; }
    template <typename T>
    constexpr bool operator!=(const T&) const { return ! std::is_same_v<Node, T>; }
};

template <char_t ch, auto result, typename... Children>
constexpr auto makeNode(char_<ch>, result_<result>, bh::tuple<Children...>) {
    return Node<ch, result, Children...>{};
}

template <typename Node, typename String, typename Result>
constexpr auto addStringToNode(Node node, String string, Result result) {
    auto extendedChildren = addStringToList(node.children, string, result);

    return makeNode(char_c<node.ch>, result_c<node.result>, extendedChildren);
}

template <typename String, typename Result, typename... Nodes>
constexpr auto addStringToList(bh::tuple<Nodes...> list, String string, Result result) {
    if constexpr (bh::is_empty(string)) {
        return list;
    } else {
        auto chMatches = [=](auto trie) { 
            return bh::front(string) == trie.ch;
        };
        auto extendedList = bh::adjust_if(list, chMatches, [](auto trie) {
            return trie;
        });
        if (extendedList == list) {
            return list;
        } else {
            return list;
        }
    }
}

}

using namespace std::literals::string_view_literals;

constexpr std::pair<const std::string_view, int> keywords[] = {
    {"do"sv,     TK::DO},
    {"end"sv,    TK::END},
    {"if"sv,     TK::IF},
    {"then"sv,   TK::THEN},
    {"else"sv,   TK::ELSE},
    {"elif"sv,   TK::ELIF},
    {"while"sv,  TK::WHILE},
    {"for"sv,    TK::FOR},
    {"in"sv,     TK::IN},
    {"repeat"sv, TK::REPEAT},
    {"until"sv,  TK::UNTIL},
    {"return"sv, TK::RETURN},
    {"break"sv,  TK::BREAK},
    {"next"sv,   TK::NEXT},
    {"local"sv,  TK::LOCAL},
    {"def"sv,    TK::DEF},
    {"class"sv,  TK::CLASS},
    {"nil"sv,    TK::NIL},
    {"true"sv,   TK::TRUE},
    {"false"sv,  TK::FALSE},
    {"or"sv,     TK::OR},
    {"and"sv,    TK::AND},
    {"not"sv,    TK::NOT}
};

}

using namespace std;
using namespace aya;

namespace bh = boost::hana;

int main(int argc, const char* argv[]) {
    (void)argc; (void)argv;
    constexpr auto res = detail::addStringToList(bh::make_tuple(detail::Node<'e', 23>{}), BOOST_HANA_STRING("abc"), bh::int_c<23>);
    detail::result_c<23ul>;
    return 0;
}
