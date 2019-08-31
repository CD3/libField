#ifndef Serialization_hpp
#define Serialization_hpp

/** @file Serialization.hpp
 * @brief Provides serialization support to library.
 * @author C.D. Clark III
 * @date 07/18/17
 */

#if SERIALIZATION_ENABLED
#ifndef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
#error \
    "Boost member template friends must be disabled in order to serialize mulati_array."
#endif

#include <boost/array.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "Field.hpp"

namespace boost
{
namespace serialization
{
template<class Archive, typename T, std::size_t N>
inline void save(Archive& ar, const boost::multi_array<T, N>& a,
                 const unsigned int version)
{
  // write storage order info first
  boost::array<multi_array_types::size_type, N> storage_order_ordering;
  boost::array<bool, N>                         storage_order_ascending;
  for (int i = 0; i < N; i++) {
    storage_order_ordering[i]  = a.storage_order().ordering(i);
    storage_order_ascending[i] = a.storage_order().ascending(i);
  }
  ar << make_array(storage_order_ordering.data(), N);
  ar << make_array(storage_order_ascending.data(), N);

  // now write the shape
  ar << make_array(a.shape(), N);

  // write data
  ar << make_array(a.data(), a.num_elements());

  // write index offsets
  ar << make_array(a.index_bases(), N);

  // write strides
  ar << make_array(a.strides(), N);
}

template<class Archive, typename T, std::size_t N>
inline void load(Archive& ar, boost::multi_array<T, N>& a,
                 const unsigned int version)
{
  // read storage order
  boost::array<multi_array_types::size_type, N> storage_order_ordering;
  boost::array<bool, N>                         storage_order_ascending;
  ar >> make_array(storage_order_ordering.data(), N);
  ar >> make_array(storage_order_ascending.data(), N);
  a.storage_ = general_storage_order<N>(storage_order_ordering.data(),
                                        storage_order_ascending.data());

  // read shape (NOTE: the storage order needs to be set *before* resizing)
  boost::array<multi_array_types::size_type, N> shape;
  ar >> make_array(shape.data(), N);
  a.resize(shape);

  // read data
  ar >> make_array(a.data(), a.num_elements());

  // read index offsets
  boost::array<multi_array_types::index, N> offsets;
  ar >> make_array(offsets.data(), N);
  a.reindex(offsets);

  // read strides
  boost::array<multi_array_types::index, N> strides;
  ar >> make_array(strides.data(), N);
  for (int i = 0; i < N; i++) a.stride_list_[i] = strides[i];
}

template<typename Archive, typename T, std::size_t N>
inline void serialize(Archive& ar, boost::multi_array<T, N>& a,
                      const unsigned int version)
{
  split_free(ar, a, version);
}

}  // namespace serialization
}  // namespace boost

#endif

#endif  // include protector
