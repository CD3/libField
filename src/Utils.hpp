
#ifndef Utils_hpp
#define Utils_hpp

#include <boost/array.hpp>
#include <boost/multi_array.hpp>
#include <boost/assert.hpp>

/** @file Utils.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 06/14/17
  */


// type traits
template< typename T >
struct is_index_cont : std::false_type {};

template< typename T, size_t N >
struct is_index_cont<T[N]> : std::true_type {};

template< >
struct is_index_cont<std::vector<int>> : std::true_type {};

template< >
struct is_index_cont<std::vector<size_t>> : std::true_type {};

template< size_t N >
struct is_index_cont<boost::array<int,N>> : std::true_type {};

template< size_t N >
struct is_index_cont<boost::array<size_t,N>> : std::true_type {};


// trait querries

template < template<typename,size_t> class ARRAY, typename T, size_t N>
constexpr int getNumDims( ARRAY<T,N> &a )
{
  return N;
};

// type generators


#endif // include protector
