#ifndef CoordinateSystem_hpp
#define CoordinateSystem_hpp

#include <sstream>
#include <type_traits>

#include <boost/array.hpp>
#include <boost/multi_array.hpp>
#include <boost/optional.hpp>

#include "Aliases.hpp"
#include "RangeDiscretizers.hpp"
#include "Utils.hpp"

using namespace boost;
using namespace RangeDiscretizers;

/** @file CoordinateSystem.hpp
  * @brief
  * @author C.D. Clark III
  * @date 06/13/17
  */

/** @class CoordinateSystem
* @brief
  * @author C.D. Clark III */
template <typename T>
using array1D = multi_array<T, 1>;
template <typename T>
using view1D = detail::multi_array::multi_array_view<T, 1>;

template <typename COORD, size_t NUMDIMS, template <typename> class ARRAY = array1D>
class CoordinateSystem {

  public:
  typedef ARRAY<COORD> axis_type;
  typedef typename axis_type::index coordinate_index_type;
  typedef CoordinateSystem<COORD, NUMDIMS, ARRAY> this_type;

  protected:
  array<std::shared_ptr<axis_type>, NUMDIMS> axes;

  public:
  template <typename... Args>
  CoordinateSystem(Args... args)
  {
    init_imp<0>(args...);
  }

  template <typename I>
  CoordinateSystem(array<I, NUMDIMS> sizes)
  {
    for (int i = 0; i < NUMDIMS; i++)
      axes[i].reset(new axis_type(boost::extents[sizes[i]]));
  }

  CoordinateSystem(array<std::shared_ptr<axis_type>, NUMDIMS> axes_)
  {
    for (int i = 0; i < NUMDIMS; i++)
      axes[i] = axes_[i];
  }

  virtual ~CoordinateSystem(){};

  auto size(int i) const
  {
    if (i < 0) {
      size_t n = 1;
      for (auto& a : axes)
        n *= a->size();

      return n;
    }

    // NUMDIMS is unsigned, so this will eval to true if i is negative
    if (i >= NUMDIMS)
      return 0ul;

    return axes[i]->size();
  }

  template <typename... Args>
  auto set(Args... args)
  {
    set_imp<0>(args...);
  };

  const auto& operator[](size_t i) const { return *axes[i]; }
  auto& operator[](size_t i) { return *axes[i]; }

  const auto& getAxis(size_t i) const { return *axes[i]; }
  auto& getAxis(size_t i) { return *axes[i]; }

  const auto getAxisPtr(size_t i) const { return axes[i]; }
  auto getAxisPtr(size_t i) { return axes[i]; }

  const auto getAxes() const { return axes; }
  auto getAxes() { return axes; }

  template <typename... Args>
  auto operator()(Args... args) const
  {
    array<COORD, NUMDIMS> c;
    getCoord_imp<0>(c, args...);
    return c;
  }

  template <typename... Args>
  auto getCoord(Args... args) const
  {
    array<COORD, NUMDIMS> c;
    getCoord_imp<0>(c, args...);
    return c;
  }

  template <int NDims>
  const auto
  slice(const detail::multi_array::index_gen<NUMDIMS, NDims>& ind) const
  {
    // Notes:
    // NUMDIMS is the number of dimensions of this coordinate system
    // NDims is the number of dimensions of the coordinate system we need to create.
    multi_array_types::index_gen indices;
    // create an array of axis views that will be used to construct the new coordinate system.
    array<std::shared_ptr<view1D<COORD> >, NDims> new_axes;
    int ii = 0;
    // loop through each axis
    for (int i = 0; i < NUMDIMS; i++) {
      // skip degenerate axes
      if (!ind.ranges_[i].degenerate_) {
        new_axes[ii].reset(new view1D<COORD>(
            axes[i]->operator[](indices[IRange(ind.ranges_[i].start_, ind.ranges_[i].finish_, ind.ranges_[i].stride_)])));
        ii++;
      }
    }
    return CoordinateSystem<COORD, NDims, view1D>(new_axes);
  }

  template <int NDims>
  auto
  slice(const detail::multi_array::index_gen<NUMDIMS, NDims>& ind)
  {
    // Notes:
    // NUMDIMS is the number of dimensions of this coordinate system
    // NDims is the number of dimensions of the coordinate system we need to create.
    multi_array_types::index_gen indices;
    // create an array of axis views that will be used to construct the new coordinate system.
    array<std::shared_ptr<view1D<COORD> >, NDims> new_axes;
    int ii = 0;
    // loop through each axis
    for (int i = 0; i < NUMDIMS; i++) {
      // skip degenerate axes
      if (!ind.ranges_[i].degenerate_) {
        new_axes[ii].reset(new view1D<COORD>(
            axes[i]->operator[](indices[IRange(ind.ranges_[i].start_, ind.ranges_[i].finish_, ind.ranges_[i].stride_)])));
        ii++;
      }
    }
    return CoordinateSystem<COORD, NDims, view1D>(new_axes);
  }

  // helper functions/implementation
  protected:
  template <int II, typename N, typename... Args>
  typename std::enable_if<is_integral<N>::value, void>::type
  init_imp(N n, Args... args)
  {
    axes[II].reset(new axis_type(boost::extents[n]));
    init_imp<II + 1>(args...);
  }

  template <int II, typename... Args>
  void init_imp(std::shared_ptr<axis_type>& axis, Args... args)
  {
    axes[II] = axis;
    init_imp<II + 1>(args...);
  }

  template <int II>
  void init_imp()
  {
    BOOST_STATIC_ASSERT_MSG(II == NUMDIMS,
        "CoordinateSystem<COORD,NUMDIMS> "
        "constructor called with wrong "
        "number of arguments.");
  }

  template <int II, typename R, typename... Args>
  typename std::enable_if<!is_same<R, none_t>::value, void>::type
  set_imp(R range, Args... args)
  {
    size_t N = axes[II]->size();
    for (int i = 0; i < N; ++i)
      axes[II]->operator[](i) = range(i, N);

    set_imp<II + 1>(args...);
  }

  template <int II, typename R, typename... Args>
  typename std::enable_if<is_same<R, none_t>::value, void>::type
      set_imp(R, Args... args)
  {
    set_imp<II + 1>(args...);
  }

  template <int II>
  void set_imp()
  {
    BOOST_STATIC_ASSERT_MSG(II == NUMDIMS,
        "CoordinateSystem<COORD,NUMDIMS> "
        "set method called with wrong "
        "number of arguments.");
  };

  template <int II, typename C, typename I, typename... Args>
  void getCoord_imp(C& c, I i, Args... args) const
  {
    c[II] = axes[II]->operator[](i);
    getCoord_imp<II + 1>(c, args...);
  }

  template <int II, typename C>
  void getCoord_imp(C& c) const
  {
    BOOST_STATIC_ASSERT_MSG(II == NUMDIMS,
        "CoordinateSystem<COORD,NUMDIMS> "
        "getCoord called with wrong "
        "number of arguments.");
  };
};

#endif // include protector
