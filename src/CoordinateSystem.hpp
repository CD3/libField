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

/** @file CoordinateSystem.hpp
  * @brief
  * @author C.D. Clark III
  * @date 06/13/17
  */

/** @class CoordinateSystem
* @brief
  * @author C.D. Clark III */
template <typename T>
using array1D = boost::multi_array<T, 1>;
template <typename T>
using view1D = boost::detail::multi_array::multi_array_view<T, 1>;

template <typename COORD, size_t NUMDIMS, template <typename> class ARRAY = array1D>
class CoordinateSystem {

  public:
  typedef ARRAY<COORD> axis_type;
  typedef typename axis_type::index coordinate_index_type;
  typedef CoordinateSystem<COORD, NUMDIMS, ARRAY> this_type;

  protected:
  std::array<std::shared_ptr<axis_type>, NUMDIMS> axes;


  public:
#if SERIALIZATION_ENABLED

template<class Archive>
void serialize( Archive &ar, const unsigned int version)
{
  ar & axes;
}

#endif

  CoordinateSystem() = default;
  CoordinateSystem(const CoordinateSystem&) = default;
  CoordinateSystem(CoordinateSystem&&) = default;

  template <typename... Args>
  CoordinateSystem(Args... args)
  {
    init_imp<0>(args...);
  }

  template <typename I>
  CoordinateSystem(std::array<I, NUMDIMS> sizes)
  {
    for (int i = 0; i < NUMDIMS; i++)
      axes[i].reset(new axis_type(boost::extents[sizes[i]]));
  }

  CoordinateSystem(std::array<std::shared_ptr<axis_type>, NUMDIMS> axes_)
  {
    for (int i = 0; i < NUMDIMS; i++)
      axes[i] = axes_[i];
  }

  virtual ~CoordinateSystem(){};

  /** Returns size of the i'th axis. */
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

  /** Set coorinate values
   *
   * This function accepts a set of Range objects that are used to
   * set the coordinate values.
   * */
  template <typename... Args>
  auto set(Args... args)
  {
    set_imp<0>(args...);
  };

  /** Return i'th axis */
  const auto& operator[](size_t i) const { return *axes[i]; }
  auto& operator[](size_t i) { return *axes[i]; }

  /** Return i'th axis */
  const auto& getAxis(size_t i) const { return *axes[i]; }
  auto& getAxis(size_t i) { return *axes[i]; }

  /** Return pointer to i'th axis */
  const auto getAxisPtr(size_t i) const { return axes[i]; }
  auto getAxisPtr(size_t i) { return axes[i]; }

  /** Return pointer to axes array */
  const auto getAxes() const { return axes; }
  auto getAxes() { return axes; }

  /** Return coordinate specified by indecies given as arguments */
  template <typename... Args>
  auto operator()(Args... args) const
  {
    std::array<COORD, NUMDIMS> c;
    getCoord_imp<0>(c, args...);
    return c;
  }

  /** Return coordinate specified by indecies given as arguments */
  template <typename... Args>
  auto getCoord(Args... args) const
  {
    std::array<COORD, NUMDIMS> c;
    getCoord_imp<0>(c, args...);
    return c;
  }

  /** Return a sliced coordinate system view based on an index generator. */
  template <int NDims>
  const auto
  slice(const boost::detail::multi_array::index_gen<NUMDIMS, NDims>& ind) const
  {
    // Notes:
    // NUMDIMS is the number of dimensions of this coordinate system
    // NDims is the number of dimensions of the coordinate system we need to create.
    boost::multi_array_types::index_gen indices;
    // create an array of axis views that will be used to construct the new coordinate system.
    std::array<std::shared_ptr<view1D<COORD> >, NDims> new_axes;
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

  /** Return a sliced coordinate system view based on an index generator. */
  template <int NDims>
  auto
  slice(const boost::detail::multi_array::index_gen<NUMDIMS, NDims>& ind)
  {
    // Notes:
    // NUMDIMS is the number of dimensions of this coordinate system
    // NDims is the number of dimensions of the coordinate system we need to create.
    boost::multi_array_types::index_gen indices;
    // create an array of axis views that will be used to construct the new coordinate system.
    std::array<std::shared_ptr<view1D<COORD> >, NDims> new_axes;
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

  /** WARNING: this does not have the same meaning as std::lower_bound.*/
  template <typename... Args>
  auto
  lower_bound(Args... args) const
  {
    std::array<int, NUMDIMS> ind;
    lower_bound_imp<0>(ind, args...);
    return ind;
  }

  template <typename... Args>
  auto
  upper_bound(Args... args) const
  {
    std::array<int, NUMDIMS> ind;
    upper_bound_imp<0>(ind, args...);
    return ind;
  }

  template <typename... Args>
  auto
  nearest(Args... args) const
  {
    std::array<int, NUMDIMS> ind;
    nearest_imp<0>(ind, args...);
    return ind;
  }

  // helper functions/implementations
  protected:
  template <int II, typename N, typename... Args>
  typename std::enable_if<std::is_integral<N>::value, void>::type
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
  typename std::enable_if<!std::is_same<R, boost::none_t>::value, void>::type
  set_imp(R range, Args... args)
  {
    size_t N = axes[II]->size();
    for (int i = 0; i < N; ++i)
      axes[II]->operator[](i) = range(i, N);

    set_imp<II + 1>(args...);
  }

  template <int II, typename R, typename... Args>
  typename std::enable_if<std::is_same<R, boost::none_t>::value, void>::type
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
  }

  template <int II, typename IND, typename C, typename... Args>
  void lower_bound_imp(IND& ind, C c, Args... args) const
  {
    ind[II] = std::upper_bound( axes[II]->begin(), axes[II]->end(), c ) - axes[II]->begin() - 1;
    lower_bound_imp<II + 1>(ind, args...);
  }

  template <int II, typename IND>
  void lower_bound_imp(IND& ind) const
  {
    BOOST_STATIC_ASSERT_MSG(II == NUMDIMS,
        "CoordinateSystem<COORD,NUMDIMS> "
        "lower_bound called with wrong "
        "number of arguments.");
  }

  template <int II, typename IND, typename C, typename... Args>
  void upper_bound_imp(IND& ind, C c, Args... args) const
  {
    ind[II] = std::upper_bound( axes[II]->begin(), axes[II]->end(), c ) - axes[II]->begin();
    upper_bound_imp<II + 1>(ind, args...);
  }

  template <int II, typename IND>
  void upper_bound_imp(IND& ind) const
  {
    BOOST_STATIC_ASSERT_MSG(II == NUMDIMS,
        "CoordinateSystem<COORD,NUMDIMS> "
        "upper_bound called with wrong "
        "number of arguments.");
  }

  template <int II, typename IND, typename C, typename... Args>
  void nearest_imp(IND& ind, C c, Args... args) const
  {
    // special cases:
    // coordinate is less than smallest
    if( c < axes[II]->operator[](0) )
    {
      ind[II] = 0;
    }
    // coordinate is greater than largest
    else if( c > axes[II]->operator[]( axes[II]->size()-1 ) )
    {
      ind[II] = axes[II]->size()-1;
    }
    else
    {
      // get index of element that is just below coordinate
      ind[II] = std::upper_bound( axes[II]->begin(), axes[II]->end(), c ) - axes[II]->begin() - 1;
      // now determine if coordinate is closer to the upper bound or lower bound
      // coord - lower bound divided by upper bound minus lower bound gives the dimensionless coordinate between 0 and 1.
      // if this is less than 0.5, we want to return the lower bound index. if itis greater than 0.5, we want to return the upper bound index
      // don't have to worry about special cases above here.
      ind[II] += 2*(c - axes[II]->operator[](ind[II]))/(axes[II]->operator[](ind[II]+1) - axes[II]->operator[](ind[II]));
    }

    nearest_imp<II + 1>(ind, args...);
  }

  template <int II, typename IND>
  void nearest_imp(IND& ind) const
  {
    BOOST_STATIC_ASSERT_MSG(II == NUMDIMS,
        "CoordinateSystem<COORD,NUMDIMS> "
        "nearest called with wrong "
        "number of arguments.");
  }
};

#endif // include protector
