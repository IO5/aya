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

namespace detail {

template <typename... Tries>
struct Matcher;
template <typename TrieNode, typename... Tries>
struct Matcher<TrieNode, Tries...> {
	static constexpr auto match(std::string_view sv) {
		if (auto result = TrieNode::match(sv))
			return result;
		return Matcher<Tries...>::match(sv);
	}
};
template <>
struct Matcher<> {
	static constexpr auto match(std::string_view sv) {
		return 0;
	}
};
template <typename... Tries>
struct Matcher<std::tuple<Tries...>> {
	static constexpr auto match(std::string_view sv) {
		return Matcher<Tries...>::match(sv);
	}
};

template<auto result>
struct TrieResult {
	static constexpr auto match(std::string_view sv) {
		return sv.empty() ? result : decltype(result)();
	}
};

template<char ch, typename... Children>
struct Trie {
	static constexpr auto match(std::string_view sv) {
		if (!sv.empty() && sv[0] == ch)
			return Matcher<Children...>::match(sv.substr(1));
		return 0;
	}
};

template <typename TrieTuple, const auto& mapping, size_t charsLeft>
struct AddTrieToList;

template <typename InputTrie, const auto& mapping, size_t charsLeft>
struct TryAddToTrie {
	using Result = InputTrie;
};

template <char ch, typename TrieTuple>
struct TrieFromTuple;
template <char ch, typename... Tries>
struct TrieFromTuple<ch, std::tuple<Tries...>> {
	using Result = Trie<ch, Tries...>;
};

template <const auto& mapping, size_t charsLeft, typename... Tries>
struct AddTrieToList<std::tuple<Tries...>, mapping, charsLeft> {
	static constexpr auto& str = std::get<0>(mapping);
	static constexpr char ch = str[str.size() - charsLeft];

	using OriginalList = std::tuple<Tries...>;
	using ExtendedList = std::tuple<
		typename TryAddToTrie<Tries, mapping, charsLeft>::Result
		...
	>;

	using Result = std::conditional_t<
		std::is_same<OriginalList, ExtendedList>::value,
		std::tuple<Tries...,
			typename TrieFromTuple<ch,
				typename AddTrieToList<
					std::tuple<>,
					mapping,
					charsLeft - 1
				>::Result
			>::Result
		>,
		ExtendedList
	>;
};
template <const auto& mapping, typename... Tries>
struct AddTrieToList<std::tuple<Tries...>, mapping, 0> {
	static constexpr auto& result = std::get<1>(mapping);
	using Result = std::tuple<Tries..., TrieResult<result>>;
};

template <const auto& mapping, size_t charsLeft, char trieCh, typename... Children>
struct TryAddToTrie<Trie<trieCh, Children...>, mapping, charsLeft> {
	static constexpr auto& str = std::get<0>(mapping);
	static constexpr char ch = str[str.size() - charsLeft];

	using Result = std::conditional_t<
		ch == trieCh,
		typename TrieFromTuple<
			ch,
			typename AddTrieToList<
				std::tuple<Children...>,
				mapping,
				charsLeft - 1
			>::Result
		>::Result,
		Trie<trieCh, Children...>
	>;
};

template <const auto& mappings, int idx>
constexpr auto StringMapping = mappings[idx];

template <typename TrieTuple, const auto& mappings, size_t idx>
struct TrieFromStringList;
template <const auto& mappings, size_t idx, typename... Tries>
struct TrieFromStringList<std::tuple<Tries...>, mappings, idx> {
	static constexpr auto& mapping = StringMapping<mappings, idx>;
	using ExtendedList = typename AddTrieToList<
		std::tuple<Tries...>, mapping, std::get<0>(mapping).size()
	>::Result;
	using Result = typename TrieFromStringList<ExtendedList, mappings, idx - 1>::Result;
};
template <const auto& mappings, typename... Tries>
struct TrieFromStringList<std::tuple<Tries...>, mappings, 0> {
	static constexpr auto& mapping = StringMapping<mappings, 0>;
	using Result = typename AddTrieToList<
		std::tuple<Tries...>, mapping, std::get<0>(mapping).size()
	>::Result;
};

}

constexpr std::pair<const std::string_view, int> keywords[] = {
	{{"abc",3}, 1},
	{{"aba",3}, 2},
	{{"bac",3}, 3},
	{{"b",1}, 4},
	{{"abac",4}, 5}
};

using namespace detail;

using namespace aya;
using namespace std;

using BigTrie = TrieFromStringList<std::tuple<>, keywords, sizeof(keywords)/sizeof(keywords[0]) - 1>::Result;

int main(int argc, const char* argv[]) {
	cout << Matcher<BigTrie>::match(argv[1]) << '\n';
    return 0;
}
