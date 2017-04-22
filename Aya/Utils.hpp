#pragma once

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <gsl>

namespace aya {

template <typename T>
using not_null = gsl::not_null<T>;

template <typename T, typename Deleter = std::default_delete<T>>
using unique_ptr = std::unique_ptr<T, Deleter>;

template <typename T, typename Deleter = std::default_delete<T>>
using nn_unique_ptr = not_null<unique_ptr<T, Deleter>>;

template <typename T>
using IntrusivePtr = boost::intrusive_ptr<T>;

//template <typename T>
//void intrusive_ptr_add_ref(const T* ptr) noexcept;
//template <typename T>
//void intrusive_ptr_release(const T* ptr) noexcept;

template<typename T, typename... Types>
using is_one_of = std::disjunction<std::is_same<T, Types>...>;
template<typename T, typename... Types>
constexpr bool is_one_of_v = is_one_of<T, Types...>::value;

}
