#ifndef Field_hpp
#define Field_hpp

#include <type_traits>
#include <typeinfo>
#include <vector>

#include <boost/multi_array.hpp>
using namespace boost;

#include "CoordinateSystem.hpp"
#include "Utils.hpp"

/** @file Field.hpp
  * @brief
  * @author C.D. Clark III
  * @date 06/13/17
  */

/** @class Field
  * @brief A class for storing data in a field.
  * @author C.D. Clark III
  */
template <typename T, std::size_t N>
using arrayND = multi_array<T, N, std::allocator<T>>;
template <typename T, std::size_t N>
using viewND = detail::multi_array::multi_array_view<T, N>;

template <typename QUANT, size_t NUMDIMS, typename COORD = QUANT, template <typename, size_t> class ARRAYND = arrayND, template <typename> class ARRAY1D = array1D>
class Field {
  public:
  typedef ARRAYND<QUANT, NUMDIMS> array_type;
  typedef typename array_type::index index_type;
  typedef CoordinateSystem<COORD, NUMDIMS, ARRAY1D> cs_type;

  protected:
  std::shared_ptr<array_type> d;
  std::shared_ptr<cs_type> cs;

  public:

  template <typename... Args>
  Field(Args... args)
  {
    cs.reset(new cs_type(args...));
    array<int, NUMDIMS> sizes({ args... });
    d.reset(new array_type(sizes));
  }
  template <typename I>
  Field(array<I,NUMDIMS> sizes)
  {
    cs.reset(new cs_type(sizes));
    d.reset(new array_type(sizes));
  }
  Field(std::shared_ptr<cs_type> cs_)
  {
    cs = cs_;

    std::vector<size_t> sizes(NUMDIMS);
    for (int i = 0; i < NUMDIMS; ++i)
      sizes[i] = cs->size(i);

    d.reset(new array_type(sizes));
  }

  Field(cs_type &cs_, array_type &d_)
  {
    d.reset(new array_type(d_));
    cs.reset(new cs_type(cs_.getAxes()));
  };
  virtual ~Field(){};

  // ELEMENT ACCESS

  // using an index container ( C array, std::vector, boost::array, etc )
  template <typename I>
  typename std::enable_if<is_index_cont<I>::value, QUANT>::type&
  operator()(I i)
  {
    return (*d)(i);
  }

  // indices given as multiple arguments
  template <typename... Args>
  QUANT&
  operator()(Args... args)
  {
    return (*d)(array<int, NUMDIMS>({ args... }));
  }

  template <typename I>
  auto operator[](I i)
  {
    return (*d)[i];
  };

  // coord system access
  auto& getCoordinateSystem() { return *cs; };
  auto getCoordinateSystemPtr() { return cs; };
  auto& getAxis(size_t i){ return cs->getAxis(i); }

  template <typename... Args>
  auto
  getCoord(Args... args) { return cs->getCoord(args...); }

  // data access
  array_type getData() { return *d; };
  QUANT* data() { return d->data(); }

  template <int NDims>
  auto
  slice(const detail::multi_array::index_gen<NUMDIMS, NDims>& ind)
  {
    // get sliced data
    auto d_ = d->operator[](ind);
    // get sliced coordinate system
    auto cs_ = cs->slice(ind);

    return Field<QUANT,NDims,COORD,viewND,view1D>(cs_,d_);
  }

};

#endif
