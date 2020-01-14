#ifndef Field_hpp
#define Field_hpp

#include <ostream>
#include <type_traits>
#include <typeinfo>
#include <vector>

#include <boost/multi_array.hpp>

#include "CoordinateSystem.hpp"
#include "Utils.hpp"

/** @class Field
 * @brief A class for storing data in a field.
 * @author C.D. Clark III
 * @date 06/13/17
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

  Field()        = default;
  Field(Field&&) = default;

  Field(const Field& f) { reset(*f.cs, *f.d); }

  template<typename... Args>
  Field(Args... args)
  {
    reset(args...);
  }
  template<typename I>
  Field(std::array<I, NUMDIMS> sizes)
  {
    reset(sizes);
  }
  Field(std::shared_ptr<cs_type> cs_) { reset(cs_); }

  Field(cs_type& cs_, array_type& d_) { reset(cs_, d_); };

  template<typename... Args>
  void reset(Args... args)
  {
    cs.reset(new cs_type(args...));
    std::array<int, NUMDIMS> sizes({args...});
    d.reset(new array_type(sizes));
  }
  template<typename I>
  void reset(std::array<I, NUMDIMS> sizes)
  {
    cs.reset(new cs_type(sizes));
    d.reset(new array_type(sizes));
  }
  void reset(std::shared_ptr<cs_type> cs_)
  {
    cs = cs_;

    std::vector<size_t> sizes(NUMDIMS);
    for (size_t i = 0; i < NUMDIMS; ++i) sizes[i] = cs->size(i);

    d.reset(new array_type(sizes));
  }

  void reset(cs_type& cs_, array_type& d_)
  {
    d.reset(new array_type(d_));
    cs.reset(new cs_type(cs_.getAxes()));
  };

  ~Field() = default;

  // ELEMENT ACCESS

  // using an index container ( C array, std::vector, std::array, etc )
  template<typename I,
           typename std::enable_if<IsIndexCont<I>::value, int>::type = 0>
  const auto& operator()(I i) const
  {
    return (*d)(i);
  }

  template<typename I,
           typename std::enable_if<IsIndexCont<I>::value, int>::type = 0>
  auto& operator()(I i)
  {
    return (*d)(i);
  }

  // indices given as separate arguments
  template<typename I, typename... Args,
           typename std::enable_if<std::is_integral<I>::value, int>::type = 0>
  const auto& operator()(I i, Args... args) const
  {
    return (*d)(std::array<I, NUMDIMS>({i, args...}));
  }

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

  // coord system access
  auto        getCoordinateSystemPtr() { return cs; };
  auto&       getCoordinateSystem() { return *cs; };
  auto&       getAxis(size_t i) { return cs->getAxis(i); }
  const auto& getCoordinateSystem() const { return *cs; };
  const auto& getAxis(size_t i) const { return cs->getAxis(i); }

  template<typename... Args>
  auto setCoordinateSystem(Args... args)
  {
    cs->set(args...);
  }

  template<typename... Args>
  auto getCoord(Args... args) const
  {
    return cs->getCoord(args...);
  }

  // returns index of stored coordinate that lower bounds the given coordinate
  template<typename... Args>
  auto lower_bound(Args... args) const
  {
    return cs->lower_bound(args...);
  }

  // returns index of stored coordinate that upper bounds the given coordinate
  template<typename... Args>
  auto upper_bound(Args... args) const
  {
    return cs->upper_bound(args...);
  }

  // returns index of stored coordinate that is closest to the given coordinate
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
   * Evaluates callable f for each coordinate and sets the field value to value
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
   * Evaluates callable f that returns an optional type (boost::optional or
   * std::optional) for each coordinate. If the optional is set, then the valu
   * eof the field is set. Otherwise, the field is left untouched. Field may be
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
   * Evaluates callable f for each indices and sets the field value to value
   * returned by callable. Callable is passed a container of indicies and a
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
   * Evaluates callable f for each coordinate index. If the optional returned by
   * f is set, the field for the coordinate is set. Otherwise, the field is left
   * untouched. Callable is passed a container of indicies and a pointer to the
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
