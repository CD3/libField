#ifndef RangeDiscretizers_hpp
#define RangeDiscretizers_hpp

/** @file RangeDiscretizers.hpp
 * @brief
 * @author C.D. Clark III
 * @date 06/16/17
 */

// a collection of range discretized.
// these calculate values in a discrete set over some range.
namespace range_discretizers
{
template<typename T>
class UniformImp
{
 public:
  UniformImp(T min, T max)
  {
    this->min = min;
    this->max = max;
  }

  auto operator()(size_t i, size_t N) const
  {
    return min + i * (1. * max - 1. * min) / (N - 1);
  }

 protected:
  T min, max;
};

// using a factory function here so that argument types can be deduced.
template<typename T, typename V>
UniformImp<T> Uniform(T min, V max)
{
  return UniformImp<T>(min, max);
}

template<typename T>
class GeometricImp
{
 public:
  GeometricImp(T min, T dx, T stretch)
  {
    this->min     = min;
    this->dx      = dx;
    this->stretch = stretch;
  }

  auto operator()(size_t i, size_t N) const
  {
    // x[0] = xmin
    // x[1] = xmin + dx
    // x[2] = xmin + dx + s*dx
    // x[3] = xmin + dx + s*dx + s*s*dx
    // x[i] = xmin + dx*\sigma s^(i-1)
    // which is the geometric series.

    return min + 1. * dx * (1 - pow(1. * stretch, i)) / (1 - stretch);
  }

 protected:
  T min, dx, stretch;
};

// using a factory function here so that argument types can be deduced.
template<typename T, typename V, typename U>
GeometricImp<T> Geometric(T min, V dx, U stretch)
{
  return GeometricImp<T>(min, dx, stretch);
}

}  // namespace range_discretizers

// pull functions into global namespace
using range_discretizers::Geometric;
using range_discretizers::Uniform;

#endif  // include protector
