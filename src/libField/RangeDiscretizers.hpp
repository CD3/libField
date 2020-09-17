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
template<typename T>
UniformImp<T> Uniform(T min, T max)
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
    assert(stretch > 1);
  }

  auto operator()(size_t i, size_t N) const
  {
    // x[0] = xmin
    // x[1] = xmin + dx
    // x[2] = xmin + dx + s*dx
    // x[3] = xmin + dx + s*dx + s*s*dx
    // x[i] = xmin + dx*\sigma s^(i-1)
    // which is the geometric series.
    if(i >= N)
      i = N-1;
    return min + 1. * dx * (1 - pow(1. * stretch, i)) / (1 - stretch);
  }

 protected:
  T min, dx;
  double stretch;
};

// using a factory function here so that argument types can be deduced.
template<typename T, typename S>
GeometricImp<T> Geometric(T min, T dx, S stretch)
{
  return GeometricImp<T>(min, dx, stretch);
}



template<typename T>
class GeometricWithPeriodImp
{
 public:
  GeometricWithPeriodImp(T min, T dx, T stretch, T period)
  {
    this->min     = min;
    this->dx      = dx;
    this->stretch = stretch;
    this->period = period;
    this->Np = 1+log( 1 - (this->period/this->dx)*(1 - stretch) )/log(stretch); // the number of points that can fit inside one period
    assert(stretch > 1);
  }

  auto operator()(size_t i, size_t N) const
  {
    // x[0] = xmin
    // x[1] = xmin + dx
    // x[2] = xmin + dx + s*dx
    // x[3] = xmin + dx + s*dx + s*s*dx
    // x[i] = xmin + dx*\sigma s^(i-1)
    // which is the geometric series.
    // determine the maximum i value of the period of the function
    // which makes it periodic
    
    if(i >= N){
      i = N-1;
    }
    
    size_t n = i / Np; // number of periods that have passed
    size_t j = i % Np; // number of steps past last period

    auto Dx =  n*this->period;
    Dx += 1. * dx * (1 - pow(1. * stretch, j)) / (1 - stretch);
    return 1.*min + Dx;
  }

 protected:
  T min, dx, period;
  size_t Np;
  double stretch;
};

// using a factory function here so that argument types can be deduced.
template<typename T, typename S>
GeometricWithPeriodImp<T> GeometricWithPeriod(T min, T dx, S stretch, T period)
{
  return GeometricWithPeriodImp<T>(min, dx, stretch, period);
}


}  // namespace range_discretizers



// pull functions into global namespace
using range_discretizers::Geometric;
using range_discretizers::Uniform;
using range_discretizers::GeometricWithPeriod;

#endif  // include protector
