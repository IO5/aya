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

using namespace aya;
using namespace std;

template<typename... Children>
struct Matcher;

template<typename Child, typename... Children>
struct Matcher<Child, Children...> {
	static constexpr int match(const char* ch) {
		if (int result = Child::match(ch))
			return result;
		return Matcher<Children...>::match(ch);
	}
};

template<>
struct Matcher<> {
	static constexpr int match(const char* ch) {
		return *ch == 0;
	}
};
template<typename T>
struct Matcher_;
template<typename... Children>
struct Matcher_<tuple<Children...>> {
	static constexpr int match(const char* ch) {
		return Matcher<Children...>::match(ch);
	}
};

template<char pattern, typename... Children>
struct Trie
{
	static constexpr char RootCh = pattern;
	static constexpr int match(const char* ch) {
		if (pattern == *ch)
			return Matcher<Children...>::match(ch+1);
		return 0;
	}
};

template <typename TrieToAdd, typename... Tries>
struct MakeTrie;

template <typename TrieToAdd, typename... Tries>
using MakeTrie_t = typename MakeTrie<TrieToAdd, Tries...>::type;

template <char RootCh, typename Tuple>
struct TrieFromTuple;

template <char RootCh, typename Tuple>
using TrieFromTuple_t = typename TrieFromTuple<RootCh, Tuple>::type;

template <char RootCh, typename... Children>
struct TrieFromTuple<RootCh, tuple<Children...>> {
	using type = Trie<RootCh, Children...>;
};

template <typename TrieA, typename TrieB>
struct TryAddToTrie;

template <char DifferentCh, char RootCh>
struct TryAddToTrie<Trie<DifferentCh>, Trie<RootCh>> {
	using type = Trie<RootCh>;
};

template <char RootCh, typename Child, typename... RootChildren>
struct TryAddToTrie<Trie<RootCh, Child>, Trie<RootCh, RootChildren...>> {
	//using type = Trie<RootCh, Child, RootChildren...>;
	using type = TrieFromTuple_t<RootCh, MakeTrie_t<Child, RootChildren...>>;
};

template <char DifferentCh, char RootCh, typename Child, typename... RootChildren>
struct TryAddToTrie<Trie<DifferentCh, Child>, Trie<RootCh, RootChildren...>> {
	using type = Trie<RootCh, RootChildren...>;
};

template <typename TrieToAdd, typename... Tries>
struct MakeTrie {
	using type = conditional_t<
		disjunction<bool_constant<TrieToAdd::RootCh == Tries::RootCh>...>::value,
		tuple<typename TryAddToTrie<TrieToAdd, Tries>::type...>,
		tuple<Tries..., TrieToAdd>
	>;
};

template <typename TrieToAdd, typename... Tries>
struct MakeTrie<TrieToAdd, tuple<Tries...>> {
	using type = conditional_t<
		disjunction<bool_constant<TrieToAdd::RootCh == Tries::RootCh>...>::value,
		tuple<typename TryAddToTrie<TrieToAdd, Tries>::type...>,
		tuple<Tries..., TrieToAdd>
	>;
};

template <const string_view& sv, size_t size, size_t idx>
struct MakeTrieFromStringImpl {
	using type = Trie<sv[idx], typename MakeTrieFromStringImpl<sv, size, idx+1>::type>;
};

template <const string_view& sv, size_t size>
struct MakeTrieFromStringImpl<sv, size, size-1> {
	using type = Trie<sv[size-1]>;
};

template <const string_view& sv>
using MakeTrieFromString = typename MakeTrieFromStringImpl<sv, sv.size(), 0>::type;

constexpr string_view keywords[] = {
	{"abc", 3},
	{"aba", 3},
	{"bac", 3}
};

template <int idx>
constexpr string_view Keyword = keywords[idx];

template <const auto& strs, size_t size>
struct MakeTrieFromStringList {
	using type = MakeTrie_t<
		MakeTrieFromString<Keyword<size-1>>,
		typename MakeTrieFromStringList<strs, size-1>::type
	>;
};

template <const auto& strs>
struct MakeTrieFromStringList<strs, 1> {
	using type = MakeTrieFromString<Keyword<0>>;
};

using BigTrie = MakeTrieFromStringList<keywords, 3>::type;
//using BigTrie = MakeTrieFromString<Keyword<0>>;
//using abc = Trie<'a', Trie<'b', Trie<'c'>>>;
//using aba = Trie<'a', Trie<'b', Trie<'a'>>>;
//using bac = Trie<'b', Trie<'a', Trie<'c'>>>;
//using BigTrie = MakeTrie_t<bac, MakeTrie_t<abc, aba>>;


int main(int argc, const char* argv[]) {


	//printf("%s", typeid(BigTrie).name());

	return Matcher_<BigTrie>::match("aba");
}