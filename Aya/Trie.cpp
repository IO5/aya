// WAITING FOR CLANG
#ifdef CLANG

#include <tuple>
#include <type_traits>
#include <typeinfo>

// #include <experimental/string_view>
namespace std {
struct string_view {
	const char* ptr;
	size_t len;
	constexpr string_view(const char* ptr, size_t len) : ptr(ptr), len(len) {}
	string_view(const char* ptr) : ptr(ptr), len(strlen(ptr)) {}
	constexpr size_t size() const { return len; }
	constexpr bool empty() const { return ptr == nullptr || len == 0; }
	constexpr const char& operator[](size_t idx) const { return ptr[idx]; }
	constexpr string_view substr(size_t idx) const { return {ptr+idx, len-idx}; }
};
}

namespace detail {

template <typename... Types> struct TypeList {};

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
struct Matcher<TypeList<Tries...>> {
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
struct TrieFromTuple<ch, TypeList<Tries...>> {
	using Result = Trie<ch, Tries...>;
};

template <const auto& mapping, size_t charsLeft, typename... Tries>
struct AddTrieToList<TypeList<Tries...>, mapping, charsLeft> {
	static constexpr auto& str = std::get<0>(mapping);
	static constexpr char ch = str[str.size() - charsLeft];

	using OriginalList = TypeList<Tries...>;
	using ExtendedList = TypeList<
		typename TryAddToTrie<Tries, mapping, charsLeft>::Result
		...
	>;

	using Result = std::conditional_t<
		std::is_same<OriginalList, ExtendedList>::value,
		TypeList<Tries..., 
			typename TrieFromTuple<ch, 
				typename AddTrieToList<
					TypeList<>, 
					mapping, 
					charsLeft - 1
				>::Result
			>::Result
		>,
		ExtendedList
	>;
};
template <const auto& mapping, typename... Tries>
struct AddTrieToList<TypeList<Tries...>, mapping, 0> {
	static constexpr auto& result = std::get<1>(mapping);
	using Result = TypeList<Tries..., TrieResult<result>>;
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
				TypeList<Children...>, 
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
struct TrieFromStringList<TypeList<Tries...>, mappings, idx> {
	static constexpr auto& mapping = StringMapping<mappings, idx>;
	using ExtendedList = typename AddTrieToList<
		TypeList<Tries...>, mapping, std::get<0>(mapping).size()
	>::Result;
	using Result = typename TrieFromStringList<ExtendedList, mappings, idx - 1>::Result; 
};
template <const auto& mappings, typename... Tries>
struct TrieFromStringList<TypeList<Tries...>, mappings, 0> {
	static constexpr auto& mapping = StringMapping<mappings, 0>;
	using Result = typename AddTrieToList<
		TypeList<Tries...>, mapping, std::get<0>(mapping).size()
	>::Result;
};

}

constexpr std::pair<const std::string_view, int> keywords[] = {
	{{"abc", 3}, 1},
	{{"aba", 3}, 2},
	{{"bac", 3}, 3},
	{{"b", 1}, 4},
	{{"abac", 4}, 5}
};

using namespace detail;

using BigTrie = TrieFromStringList<TypeList<>, keywords, sizeof(keywords)/sizeof(keywords[0]) - 1>::Result;
//Trie<'a', Trie<'b', TrieResult<5>>>;

int main(int argc, const char* argv[]) {
	//printf("%s", typeid(TrieFromStringList<keywords>).name());
	//using Test = Dupa;
	//printf("%s", typeid(Test).name());
	printf("%d\n", Matcher<BigTrie>::match(argv[1]));
	return Matcher<BigTrie>::match(argv[0]);
}
#endif