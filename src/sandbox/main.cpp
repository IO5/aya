//#include "Token.hpp"
using char_t = char;

#include <iostream>
#include <boost/hana/type.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/string.hpp>
#include <boost/hana/adjust_if.hpp>
#include <boost/hana/transform.hpp>
#include <boost/hana/drop_front_exactly.hpp>
#include <boost/hana/append.hpp>
#include <boost/hana/front.hpp>
#include <boost/hana/equal.hpp>
#include <string_view>
#include <type_traits>
#include <cstdint>


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

template <typename T>
constexpr bh::integral_constant<int, 0> noMatch_c{};

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

template <typename String, typename Result, typename... Nodes>
constexpr auto addStringToList(bh::tuple<Nodes...> list, String string, Result result);

template <typename Node, typename String, typename Result>
constexpr auto addStringToNode(Node node, String string, Result result) {
    // at last charcter -> set result
    if constexpr (bh::is_empty(string)) {
        // result already set -> duplicate match
        static_assert(node.result == 0, "duplicate entry inside the trie");

        return makeNode(char_c<node.ch>, result, node.children);
    } else {
        auto extendedChildren = addStringToList(node.children, string, result);

        return makeNode(char_c<node.ch>, result_c<node.result>, extendedChildren);
    }
}

template <typename String, typename Result, typename... Nodes>
constexpr auto addStringToList(bh::tuple<Nodes...> list, String string, Result result) {
    // false positives on unused variable
    if constexpr (bh::is_empty(string)) {
        (void)result;
        return list;
    } else {
        auto stringTail = bh::drop_front_exactly(string);
        // if i use string instead of String{} I get internal compiler error lel
        constexpr char ch = bh::front(String{}).value;
        (void)ch;
        auto extendedList = bh::transform(list, [&](auto node) constexpr { 
            if constexpr (node.ch == ch) {
                return addStringToNode(node, stringTail, result);
            } else {
                return node;
            }
        });
        if constexpr (extendedList == list) {
            auto emptyNode = makeNode(char_c<bh::front(string)>, noMatch_c<Result>, bh::make_tuple()); 
            auto newNode = addStringToNode(emptyNode, stringTail, result);
            return bh::append(list, newNode);
        } else {
            return extendedList;
        }
    }
}

template <typename... Nodes>
constexpr auto match(bh::tuple<Nodes...> list, const char_t* str) {
    if constexpr (bh::is_empty(list)) {
        (void)str;
        return 0;
    } else {
        auto head = bh::front(list);
        if (*str == head.ch) {
            return match(head, str);
        }
        return match(bh::drop_front_exactly(list), str);
    }
}
template <typename Node>
constexpr auto match(Node node, const char_t* str) {
    if (auto result = match(node.children, str+1)) {
        return result;
    } else {
        return node.result;
    }
}

}

//using namespace std::literals::string_view_literals;

//constexpr std::pair<const std::string_view, int> keywords[] = {
    //{"do"sv,     TK::DO},
    //{"end"sv,    TK::END},
    //{"if"sv,     TK::IF},
    //{"then"sv,   TK::THEN},
    //{"else"sv,   TK::ELSE},
    //{"elif"sv,   TK::ELIF},
    //{"while"sv,  TK::WHILE},
    //{"for"sv,    TK::FOR},
    //{"in"sv,     TK::IN},
    //{"repeat"sv, TK::REPEAT},
    //{"until"sv,  TK::UNTIL},
    //{"return"sv, TK::RETURN},
    //{"break"sv,  TK::BREAK},
    //{"next"sv,   TK::NEXT},
    //{"local"sv,  TK::LOCAL},
    //{"def"sv,    TK::DEF},
    //{"class"sv,  TK::CLASS},
    //{"nil"sv,    TK::NIL},
    //{"true"sv,   TK::TRUE},
    //{"false"sv,  TK::FALSE},
    //{"or"sv,     TK::OR},
    //{"and"sv,    TK::AND},
    //{"not"sv,    TK::NOT}
//};

}

using namespace std;
using namespace aya;

namespace bh = boost::hana;

using Trie1 = decltype(detail::addStringToList(bh::make_tuple(), bh::string_c<'a','b','c'>,     bh::int_c<1>));
using Trie2 = decltype(detail::addStringToList(Trie1{},          bh::string_c<'a','b','a'>,     bh::int_c<2>));
using Trie3 = decltype(detail::addStringToList(Trie2{},          bh::string_c<'b','a','c'>,     bh::int_c<3>));
using Trie4 = decltype(detail::addStringToList(Trie3{},          bh::string_c<'b'>,             bh::int_c<4>));
using Trie5 = decltype(detail::addStringToList(Trie4{},          bh::string_c<'a','b','a','c'>, bh::int_c<5>));

template <typename T>
void print(T& t) {
    static_assert(t == false);
}

int main(int argc, const char* argv[]) {
    if (argc != 2)
        return -1;
    return match(Trie5{}, argv[1]);
}
