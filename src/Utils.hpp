
#ifndef Utils_hpp
#define Utils_hpp

#include <boost/array.hpp>
#include <boost/assert.hpp>
#include <boost/multi_array.hpp>

/** @file Utils.hpp
 * @brief
 * @author C.D. Clark III
 * @date 06/14/17
 */

// type traits
template<typename T>
struct IsIndexCont : std::false_type {
};

template<typename T, size_t N>
struct IsIndexCont<T[N]> : std::true_type {
};

template<>
struct IsIndexCont<std::vector<int>> : std::true_type {
};

template<>
struct IsIndexCont<std::vector<size_t>> : std::true_type {
};

template<size_t N>
struct IsIndexCont<boost::array<int, N>> : std::true_type {
};

template<size_t N>
struct IsIndexCont<boost::array<size_t, N>> : std::true_type {
};

template<size_t N>
struct IsIndexCont<std::array<int, N>> : std::true_type {
};

template<size_t N>
struct IsIndexCont<std::array<size_t, N>> : std::true_type {
};

// trait querries

template<template<typename, size_t> class ARRAY, typename T, size_t N>
constexpr int getNumDims(ARRAY<T, N>& a)
{
  return N;
};

template<class F, class... Args>
struct IsCallable {
  template<class U>
  static auto test(U* p)
      -> decltype((*p)(std::declval<Args>()...), void(), std::true_type());
  template<class U>
  static auto test(...) -> decltype(std::false_type());

  static constexpr bool VALUE = decltype(test<F>(0))::value;
};

// type generators

#endif  // include protector
