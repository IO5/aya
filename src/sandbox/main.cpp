#include "Value.hpp"
#include "CFunction.hpp"
#include "Object.hpp"

#include <iostream>
#include <memory>
#include <boost/lexical_cast.hpp>
#include <string_view>
#include <tuple>
#include <array>
#include <type_traits>
#include <optional>

namespace aya {

namespace detail {

template <typename... Tries>
struct TrieList;

template <char_t ch, typename... Children>
struct Trie {
    template <typename R, typename F>
    static constexpr R match(const char* ptr, F&& stopCond) {
        if (*ptr == ch)
            return TrieList<Children...>::template match<R>(ptr + 1, std::forward<F>(stopCond));
        return R{};
    }
};

template <auto result>
struct TrieResult {
    template <typename R, typename F>
    static constexpr R match(const char* ptr, F&& stopCond) {
        return stopCond(ptr) ? result : R{};
    }
};

template <typename Head, typename... Tries>
struct TrieList<Head, Tries...> {
    template <typename R, typename F>
    static constexpr R match(const char* ptr, F&& stopCond) {
        if (auto result = Head::template match<R>(ptr, std::forward<F>(stopCond)))
            return result;
        return TrieList<Tries...>::template match<R>(ptr, std::forward<F>(stopCond));
    }
};
template <>
struct TrieList<> {
    template <typename R, typename F>
    static constexpr R match(const char*, F&&) {
        return R{};
    }
};

template <typename T> struct is_trie_result : std::false_type {};
template <auto _> struct is_trie_result<TrieResult<_>> : std::true_type {};

template <typename List, const auto& mapping, size_t charsLeft>
struct add_trie_to_list;

template <typename InputTrie, char_t ch, const auto& mapping, size_t charsLeft>
struct try_add_to_trie_impl {
    using type = InputTrie;
};
template <typename InputTrie, const auto& mapping, size_t charsLeft>
struct try_add_to_trie {
    static constexpr auto& str = std::get<0>(mapping);
    static constexpr char ch = str[str.size() - charsLeft];

    using type = typename try_add_to_trie_impl<InputTrie, ch, mapping, charsLeft>::type;
};

template <char ch, typename TrieList>
struct make_trie_from_list;
template <char ch, typename... Tries>
struct make_trie_from_list<ch, TrieList<Tries...>> {
    using type = Trie<ch, Tries...>;
};

template <const auto& mapping, size_t charsLeft, typename... Tries>
struct add_trie_to_list<TrieList<Tries...>, mapping, charsLeft> {
    static constexpr auto& str = std::get<0>(mapping);
    static constexpr char ch = str[str.size() - charsLeft];

    using OriginalList = TrieList<Tries...>;
    // try add to each child
    using ExtendedList = TrieList<
            typename try_add_to_trie<Tries, mapping, charsLeft>::type
            ...
        >;
    using NewTrie = typename make_trie_from_list<
            ch,
            typename add_trie_to_list<TrieList<>, mapping, charsLeft - 1>::type
        >::type;

    // if no one matched append new to the list 
    using type = std::conditional_t<
            std::is_same<OriginalList, ExtendedList>::value,
            TrieList<Tries..., NewTrie>,
            ExtendedList
        >;
};
template <const auto& mapping, typename... Tries>
struct add_trie_to_list<TrieList<Tries...>, mapping, 0> {
    static constexpr auto result = std::get<1>(mapping);

    static_assert(! std::disjunction_v<is_trie_result<Tries>...>, "duplicate words inside trie");

    using type = TrieList<Tries..., TrieResult<result>>;
};

template <const auto& mapping, size_t charsLeft, char ch, typename... Children>
struct try_add_to_trie_impl<Trie<ch, Children...>, ch, mapping, charsLeft> {
    using ExtendedChildrenList = typename add_trie_to_list<
        TrieList<Children...>,
        mapping,
        charsLeft - 1
    >::type;

    using type = typename make_trie_from_list<ch, ExtendedChildrenList>::type;
};

template <const auto& mappings, int idx>
constexpr auto StringMapping = mappings[idx];

template <typename List, const auto& mappings, size_t idx>
struct make_trie_from_strings {
    static constexpr auto& mapping = StringMapping<mappings, idx>;
    static constexpr auto& str = std::get<0>(mapping);
    static constexpr auto result = std::get<1>(mapping);

    using ExtendedList = typename add_trie_to_list<List, mapping, str.size()>::type;
    using type = typename make_trie_from_strings<ExtendedList, mappings, idx - 1>::type;
};
template <typename List, const auto& mappings>
struct make_trie_from_strings<List, mappings, 0> {
    static constexpr auto& mapping = StringMapping<mappings, 0>;
    static constexpr auto& str = std::get<0>(mapping);

    using type = typename add_trie_to_list<List, mapping, str.size()>::type;
};

}

template <const auto& mappings>
class Trie {
    using type = typename detail::make_trie_from_strings<
        detail::TrieList<>,
        mappings,
        sizeof(mappings)/sizeof(mappings[0]) - 1 // last index
        >::type;

    using ResultType = std::remove_reference_t<decltype(std::get<1>(mappings[0]))>;

public:
    template <typename F>
    static constexpr ResultType match(const char* ptr, F&& stopCond) {
        return type::template match<ResultType>(ptr, std::forward<F>(stopCond));
    }
    static constexpr ResultType match(const char* ptr) {
        return match(ptr, [](auto* ptr){ return !*ptr; });
    }
};

constexpr std::pair<const std::string_view, int> keywords[] = {
    {{"abc",3}, 1},
    {{"aba",3}, 2},
    {{"bac",3}, 3},
    {{"b",1}, 4},
    {{"abac",4}, 5}
};

}

using namespace std;
using namespace aya;


int main(int argc, const char* argv[]) {
    if (argc != 2)
        return -1;

    auto result = Trie<keywords>::match(argv[1]);
    cout << result << '\n';
    return 0;
}
