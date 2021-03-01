#ifndef Field_hpp
#define Field_hpp

/** @file src/libField/Field.hpp
 */

#include <ostream>
#include <type_traits>
#include <typeinfo>
#include <vector>
#include <array>

#include <boost/multi_array.hpp>

#include "CoordinateSystem.hpp"
#include "Utils.hpp"

/** @class Field
 * @brief A class for storing data in a field.
 * @author C.D. Clark III
 * @date 06/13/17
 *
 * A field is a quantity defined at every point in space. For example, the temperature
 * distribution throughout a solid, the pressure at every point in a room, or the density of a fluid.
 * A field may be 1-, 2-, or 3-dimensional, and is parameterized by a coordinate system.
 *
 * This class associates a coordinate system with a multi-dimensional array. Field elements are
 * allocated in a single, multi-dimensional array. And a CoordinateSystem is allocated for the coordinates.
 */
template<typename T, std::size_t N>
using arrayND = boost::multi_array<T, N, std::allocator<T>>;
template<typename T, std::size_t N>
using viewND = boost::detail::multi_array::multi_array_view<T, N>;

template<typename QUANT, size_t NUMDIMS, typename COORD = QUANT,
         template<typename, size_t> class ARRAYND = arrayND,
         template<typename> class ARRAY1D         = array1D>
class Field
{
 public:
  typedef ARRAYND<QUANT, NUMDIMS>                   array_type;
  typedef typename array_type::index                index_type;
  typedef CoordinateSystem<COORD, NUMDIMS, ARRAY1D> cs_type;

 protected:
  std::shared_ptr<array_type> d;
  std::shared_ptr<cs_type>    cs;

 protected:
  /**
   * @internal
   * Utility function for converting 1d index to an Nd
   * array of indices.
   */
  auto _1d2nd(size_t i) const
  {
    auto shape = d->shape();

    std::array<size_t, NUMDIMS> ind;
    int                         NN = shape[0];
    for (size_t j = 1; j < NUMDIMS; ++j) NN *= shape[j];
    for (size_t j = 0; j < NUMDIMS; ++j) {
      NN /= shape[j];
      ind[j] = i / NN;
      i -= ind[j] * NN;
    }
    return ind;
  }

 public:
#if SERIALIZATION_ENABLED
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar& d;
    ar& cs;
  }
#endif

  /**
   * @brief Create an empty field with no elements allocated.
   *
   * Memory can be allocated later with Field::reset() method. The default constructor is provided to
   * support storing fields in some containers that require a default constructor.
   */
  Field()        = default;
  Field(Field&&) = default;
  ~Field() = default;

  Field(const Field& f) { reset(*f.cs, *f.d); }

  /**
   * @brief Create a new field and allocate memory for grid defined by dims.
   *
   * @param dims a list of sizes for each dimension.
   *
   * This constructor allows the caller to pass dimension sizes directly to the constructor.
   * 
   * @code
   * libField<double,2> f(10,20);
   * @endcode
   *
   * This will create a field with 200 elements, 10 along the first dimension, 20 along the second.
   *
   */
  template<typename... Dims>
  Field(Dims... dims)
  {
    reset(dims...);
  }


  /**
   * @brief Create a new field and allocate memory for the grid defined sizes.
   *
   * @param sizes an array of integers specifying the size of the field along each dimension.
   */
  template<typename I>
  Field(std::array<I, NUMDIMS> sizes)
  {
    reset(sizes);
  }
  Field(std::shared_ptr<cs_type> cs_) { reset(cs_); }

  Field(cs_type& cs_, array_type& d_) { reset(cs_, d_); };

  /**
   * @brief Reallocate a field with new dimensions.
   *
   * @param dims The size of the new field along each dimension.
   */
  template<typename... Dims>
  void reset(Dims... dims)
  {
    cs = std::make_shared<cs_type>(dims...);
    std::array<size_t, NUMDIMS> sizes({static_cast<size_t>(dims)...});
    d = std::make_shared<array_type>(sizes);
  }


  /**
   * @brief Reallocate a field with new dimensions.
   *
   * @param sizes An array of the new field sizes along each dimension.
   */
  template<typename I>
  void reset(std::array<I, NUMDIMS> sizes)
  {
    cs = std::make_shared<cs_type>(sizes);
    d = std::make_shared<array_type>(sizes);
  }


  /**
   * @brief Reallocate a field from an existing coordinate system.
   *
   * @param cs_ a shared pointer to an existing coordinate system.
   *
   * New memory will be allocated for the field elements, but not for
   * the coordinate system.
   */
  void reset(std::shared_ptr<cs_type> cs_)
  {
    cs = cs_;

    std::vector<size_t> sizes(NUMDIMS);
    for (size_t i = 0; i < NUMDIMS; ++i) sizes[i] = cs->size(i);

    d = std::make_shared<array_type>(sizes);
  }

  /**
   * @brief Reallocate a field from an existing coordinate system and field elements.
   *
   * @param cs_ a shared pointer to an existing coordinate system.
   * @param d_ a shared pointer to an existing array of field elements.
   *
   * No new memory is allocated. References to existing field elements
   * and coordinate system will be used.
   */
  void reset(cs_type& cs_, array_type& d_)
  {
    d = std::make_shared<array_type>(d_);
    cs = std::make_shared<cs_type>(cs_.getAxes());
  };


  // ELEMENT ACCESS

  /**
   * @brief Return const reference to an element of field with given index.
   * @param i An array-like container of indices.
   *
   * Example:
   *
   * The elements of a 2-dimensional array can be accessed using a 2-element vector.
   * @code
   * Field<double,2> f(10,20);
   * ...
   * std::vector<int> ind;
   *
   * ind[0] = 2;
   * ind[1] = 4;
   *
   * double val = f(ind); // get the (2,4) element of the field.
   * @endcode
   *
   * Any container that provides a subscript operator (operator[](int)) can be used.
   */
  template<typename I,
           typename std::enable_if<IsIndexCont<I>::value, int>::type = 0>
  const auto& operator()(I i) const
  {
    return (*d)(i);
  }

  /**
   * @brief Return a reference to an element of field with given index.
   * @param i An array-like container of indices.
   *
   * Example:
   *
   * The elements of a 2-dimensional array can be accessed using a 2-element vector.
   * @code
   * Field<double,2> f(10,20);
   * ...
   * std::vector<int> ind;
   *
   * ind[0] = 2;
   * ind[1] = 4;
   *
   * f(ind) = 10; // set the (2,4) element of the field.
   * @endcode
   *
   * Any container that provides a subscript operator (operator[](int)) can be used.
   */
  template<typename I,
           typename std::enable_if<IsIndexCont<I>::value, int>::type = 0>
  auto& operator()(I i)
  {
    return (*d)(i);
  }

  /**
   * @brief Return a const reference to an element of field with given index.
   * @param i the index of the element along the first dimension.
   * @param args the indexes of the element along the remaining dimensions.
   *
   * This function is a variadic template that allow a natural access to the field elements using the operator()(), rather
   * than having to use an index container..
   *
   * Example:
   *
   * @code
   * Field<double,2> f(10,20);
   * ...
   *
   * double val = f(2,4); // get the (2,4) element of the field.
   * @endcode
   */
  template<typename I, typename... Args,
           typename std::enable_if<std::is_integral<I>::value, int>::type = 0>
  const auto& operator()(I i, Args... args) const
  {
    return (*d)(std::array<I, NUMDIMS>({i, args...}));
  }

  /**
   * @brief Return a reference to an element of field with given index.
   * @param i the index of the element along the first dimension.
   * @param args the indexes of the element along the remaining dimensions.
   *
   *
   * This function is a variadic template that allow a natural access to the field elements using the operator()(), rather
   * than having to use an index container..
   *
   * Example:
   *
   * @code
   * Field<double,2> f(10,20);
   * ...
   *
   * f(2,4) = 10; // set the (2,4) element of the field.
   * @endcode
   */
  template<typename I, typename... Args,
           typename std::enable_if<std::is_integral<I>::value, int>::type = 0>
  auto& operator()(I i, Args... args)
  {
    return (*d)(std::array<I, NUMDIMS>({i, args...}));
  }

  template<typename I>
  auto operator[](I i) const
  {
    return (*d)[i];
  }

  /**
   * @brief Return a shared pointer to the coordinate system used by the field.
   */
  auto        getCoordinateSystemPtr() { return cs; };

  /**
   * @brief Return a reference to the coordinate system used by the field.
   */
  auto&       getCoordinateSystem() { return *cs; };

  /**
   * @brief Return a reference to the i'th axis in the coordinate system used by the field.
   * @param i The index (zero-offset) of the axis to return.
   */
  auto&       getAxis(size_t i) { return cs->getAxis(i); }
  
  /**
   * @brief Return a const reference to the coordinate system used by the field.
   */
  const auto& getCoordinateSystem() const { return *cs; };
  /**
   * @brief Return a const reference to the i'th axis in the coordinate system used by the field.
   * @param i The index (zero-offset) of the axis to return.
   */
  const auto& getAxis(size_t i) const { return cs->getAxis(i); }

  /**
   * @brief Set the coordinates of the coordinate system.
   * @param args Coordinates specification. The arguments are passed directly to CoordinateSystem::set() of the coordinate system used by the field.
   *
   * Coordinate system coordinate are set using a range discretizer. See range_discretizers::UnitformImp<T> for example.
   *
   * Example:
   *
   * To configure a field over the range [-1,1] along the x direction and [0:4] along the y direction,
   * @code
   * Field<double,2> f(10,20);
   * f.setCoordinateSystem( Uniform(-1,1), Uniform(0,4) );
   * @endcode
   */
  template<typename... Args>
  auto setCoordinateSystem(Args... args)
  {
    cs->set(args...);
  }

  /**
   * @brief Return the coordinate element specified by args.
   * @param args The index of the coordinate to retrief. The arguments are passed directly to CoordinateSystem::getCoord() of the coordinate system used by the field.
   *
   * Example:
   *
   * @code
   * Field<double,2> f(10,20);
   * ...
   *
   * auto coord = f.getCoord(2,4); // get the coordinate for the field element at (2,4)
   * double x = coord[0]; // get the x coordinate;
   * double y = coord[1]; // get the x coordinate;
   * @endcode
   */
  template<typename... Args>
  auto getCoord(Args... args) const
  {
    return cs->getCoord(args...);
  }

  /**
   * @brief Returns index of stored coordinate that lower bounds the given coordinate.
   *
   * This function forwards the arguments to Coordinate::lower_bound() of the coordinate system used by the field.
   * Along with upper_bound, it is useful for finding a range that bounds the given coordinate.
   */
  template<typename... Args>
  auto lower_bound(Args... args) const
  {
    return cs->lower_bound(args...);
  }

  /**
   * @brief Returns index of stored coordinate that upper bounds the given coordinate.
   *
   * This function forwards the arguments to Coordinate::upper_bound() of the coordinate system used by the field.
   * Along with lower_bound, it is useful for finding a range that bounds the given coordinate.
   */
  template<typename... Args>
  auto upper_bound(Args... args) const
  {
    return cs->upper_bound(args...);
  }

  /**
   * @brief returns index of stored coordinate that is closest to the given coordinate.
   */
  template<typename... Args>
  auto nearest(Args... args) const
  {
    return cs->nearest(args...);
  }

  // data access
  auto        getDataPtr() { return d; };
  const auto& getData() const { return *d; };
  auto&       getData() { return *d; };

  const auto data() const { return d->data(); }
  auto       data() { return d->data(); }

  template<int NDims>
  const auto slice(
      const boost::detail::multi_array::index_gen<NUMDIMS, NDims>& ind) const
  {
    // get sliced data
    auto d_ = d->operator[](ind);
    // get sliced coordinate system
    auto cs_ = cs->slice(ind);

    return Field<QUANT, NDims, COORD, viewND, view1D>(cs_, d_);
  }

  template<int NDims>
  auto slice(const boost::detail::multi_array::index_gen<NUMDIMS, NDims>& ind)
  {
    // get sliced data
    auto d_ = d->operator[](ind);
    // get sliced coordinate system
    auto cs_ = cs->slice(ind);

    return Field<QUANT, NDims, COORD, viewND, view1D>(cs_, d_);
  }

  auto size() const { return d->num_elements(); }
  auto size(int i) const { return d->shape()[i]; }

  /**
   * @brief Set each element of a field using a callable that takes an array-like container of *coordinates* as an argument and returns the element's value.
   *
   * This function evaluates the callable f for each coordinate and sets the field value to value
   * returned by callable. Field may be evaluated in PARRALLEL. Callable should
   * NOT depend on the order of being called.
   *
   * @param f a callable object (function, funtor, lambda, std::function, etc.)
   * that accepts one argument and returns a value.
   *
   * Argument passed to callable f will be an array of coordinates.
   *
   */
  template<typename F>
  auto set_f(F f) -> decltype((*d)(0) = f(cs->getCoord(_1d2nd(0))), void())
  {
    auto N = d->num_elements();
#pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
      auto ind             = this->_1d2nd(i);
      d->  operator()(ind) = f(cs->getCoord(ind));
    }
  }

  /**
   * @brief Set each element of a field using a callable that takes an array-like container of *coordinates* as an argument and returns the element's value.
   *
   * This function evaluates the callable f that returns an optional type (boost::optional or
   * std::optional) for each coordinate. If the optional is set, then the value of
   * of the field element is set. Otherwise, the field is left untouched. Field may be
   * evaluated in PARRALLEL. Callable should NOT depend on the order of being
   * called.
   *
   * @param f a callable object (function, funtor, lambda, std::function, etc.)
   * that accepts one argument and returns an optional value.
   *
   * Argument passed to callable f will be an array of coordinates.
   *
   */
  template<typename F>
  auto set_f(F f)
      -> decltype((bool)f(cs->getCoord(_1d2nd(0))),
                  (*d)(0) = f(cs->getCoord(_1d2nd(0))).value(), void())
  {
    auto N = d->num_elements();
#pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
      auto ind = this->_1d2nd(i);
      auto val = f(cs->getCoord(ind));
      if (val) d->operator()(ind) = val.value();
    }
  }

  /**
   * @brief Set each element of a field using a callable that takes an array-like container of *indices* and a pointer to a coordinate system as arguments and returns the element's value.
   *
   * Evaluates callable f for each indices and sets the field value to value
   * returned by callable. Callable is passed a container of indices and a
   * pointer to the coordinate system. Field may be evaluated in PARRALLEL.
   * Callable should NOT depend on the order of being called.
   *
   * @param f a callable object (function, funtor, lambda, std::function, etc.)
   * that accepts two arguments and returns a value.
   *
   * Arguments passed to callable f will be an array of indices, and a pointer
   * to the coordinate system.
   *
   */
  template<typename F>
  auto set_f(F f) -> decltype((*d)(0) = f(_1d2nd(0), cs), void())
  {
    auto N = d->num_elements();
#pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
      auto ind             = this->_1d2nd(i);
      d->  operator()(ind) = f(ind, cs);
    }
  }

  /**
   * @brief Set each element of a field using a callable that takes an array-like container of *indices* and a pointer to a coordinate system as arguments and returns the element's value.
   *
   * Evaluates callable f for each coordinate index. If the optional returned by
   * f is set, the field for the coordinate is set. Otherwise, the field is left
   * untouched. Callable is passed a container of indices and a pointer to the
   * coordinate system. Field may be evaluated in PARRALLEL. Callable should NOT
   * depend on the order of being called.
   *
   * @param f a callable object (function, funtor, lambda, std::function, etc.)
   * that accepts two arguments and returns a value.
   *
   * Arguments passed to callable f will be an array of indices, and a pointer
   * to the coordinate system.
   *
   */
  template<typename F>
  auto set_f(F f) -> decltype((bool)f(_1d2nd(0), cs),
                              (*d)(0) = f(_1d2nd(0), cs).value(), void())
  {
    auto N = d->num_elements();
#pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
      auto ind = this->_1d2nd(i);
      auto val = f(ind, cs);
      if (val) d->operator()(ind) = val.value();
    }
  }

  // NOTE: we wanted to combined set and set_f into a single function, but this
  // isn't possible in general. We cannot assume that the set_f version should
  // be called if a function is passed in, because the user may actually want to
  // store the functions in the field.

  /**
   * @brief Set all elements of a field to the value specified.
   * @param q the value to set each element to.
   *
   * The method will set element values in parallel using OpenMP.
   *
   */
  template<typename Q>
  auto set(Q q)
  {
    auto N = d->num_elements();
#pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
      auto ind             = this->_1d2nd(i);
      d->  operator()(ind) = std::move(q);
    }
  }

  // operator overloads

  friend std::ostream& operator<<(std::ostream& output, const Field& F)
  {
    auto N        = F.d->num_elements();
    auto last_ind = F._1d2nd(0);
    for (size_t i = 0; i < N; ++i) {
      auto ind = F._1d2nd(i);
      // we want to print out blank lines whenever an index gets reset
      for (size_t j = 0; j < NUMDIMS; ++j)
        if (ind[j] < last_ind[j]) output << "\n";

      for (size_t j = 0; j < NUMDIMS; ++j)
        output << F.cs->getAxis(j)[ind[j]] << " ";
      output << F.d->operator()(ind) << "\n";

      last_ind = ind;
    }
    return output;
  }

  /**
   * @brief Set the elements of a field to the value given.
   *
   * Field elements are set in parallel using OpenMP.
   */
  template<typename Q>
  Field& operator=(const Q& q)
  {
    auto N = d->num_elements();
#pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
      auto ind             = this->_1d2nd(i);
      d->  operator()(ind) = q;
    }
    return *this;
  }

  /**
   * @brief Add a constant value to each element in the field.
   * @param The value to add to each element.
   *
   * Field elements are updated in parallel using OpenMP.
   */
  template<typename Q>
  Field& operator+=(const Q& q)
  {
    auto N = d->num_elements();
#pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
      auto ind = this->_1d2nd(i);
      d->  operator()(ind) += q;
    }
    return *this;
  }

  /**
   * @brief Subtract a constant value to each element in the field.
   * @param The value to subtract from each element.
   *
   * Field elements are updated in parallel using OpenMP.
   */
  template<typename Q>
  Field& operator-=(const Q& q)
  {
    auto N = d->num_elements();
#pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
      auto ind = this->_1d2nd(i);
      d->  operator()(ind) -= q;
    }
    return *this;
  }

  /**
   * @brief Multiply each element in a field by a constant value.
   * @param The value to multiply each element by.
   *
   * Field elements are updated in parallel using OpenMP.
   */
  template<typename Q>
  Field& operator*=(const Q& q)
  {
    auto N = d->num_elements();
#pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
      auto ind = this->_1d2nd(i);
      d->  operator()(ind) *= q;
    }
    return *this;
  }

  /**
   * @brief Divide each element in a field by a constant value.
   * @param The value to divide each element by.
   *
   * Field elements are updated in parallel using OpenMP.
   */
  template<typename Q>
  Field& operator/=(const Q& q)
  {
    auto N = d->num_elements();
#pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
      auto ind = this->_1d2nd(i);
      d->  operator()(ind) /= q;
    }
    return *this;
  }

  Field& operator=(Field f)
  {
    d.swap(f.d);
    cs.swap(f.cs);
    return *this;
  }

  /**
   * @brief Add the element of a second field to each element of the field.
   * @param The field containing elements to be added to this field.
   *
   * Field elements are updated in parallel using OpenMP.
   *
   * Fields must be of the size.
   */
  Field& operator+=(const Field& f)
  {
    BOOST_ASSERT(f.size() == this->size());
    auto N = d->num_elements();
#pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
      auto ind = this->_1d2nd(i);
      d->  operator()(ind) += f(ind);
    }
    return *this;
  }

  /**
   * @brief Subtract the element of a second field from each element of the field.
   * @param The field containing elements to be subtracted from this field.
   *
   * Field elements are updated in parallel using OpenMP.
   *
   * Fields must be of the size.
   */
  Field& operator-=(const Field& f)
  {
    BOOST_ASSERT(f.size() == this->size());
    auto N = d->num_elements();
#pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
      auto ind = this->_1d2nd(i);
      d->  operator()(ind) -= f(ind);
    }
    return *this;
  }

  /**
   * @brief Multiply each element in the field by the corresponding element in a second field.
   * @param The field containing elements to be multiply by.
   *
   * Field elements are updated in parallel using OpenMP.
   *
   * Fields must be of the size.
   */
  Field& operator*=(const Field& f)
  {
    BOOST_ASSERT(f.size() == this->size());
    auto N = d->num_elements();
#pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
      auto ind = this->_1d2nd(i);
      d->  operator()(ind) *= f(ind);
    }
    return *this;
  }

  /**
   * @brief Divide each element in the field by the corresponding element in a second field.
   * @param The field containing elements to divide by.
   *
   * Field elements are updated in parallel using OpenMP.
   *
   * Fields must be of the size.
   */
  Field& operator/=(const Field& f)
  {
    BOOST_ASSERT(f.size() == this->size());
    auto N = d->num_elements();
#pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
      auto ind = this->_1d2nd(i);
      d->  operator()(ind) /= f(ind);
    }
    return *this;
  }
};

#endif
