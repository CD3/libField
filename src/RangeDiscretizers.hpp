#ifndef RangeDiscretizers_hpp
#define RangeDiscretizers_hpp

/** @file RangeDiscretizers.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 06/16/17
  */


// a collection of range discretized.
// these calculate values in a discrete set over some range.
namespace RangeDiscretizers
{

  template<typename T>
  class Uniform_imp
  {
    public:
      Uniform_imp( T min, T max )
      {
        this->min = min;
        this->max = max;
      }

      T operator()(size_t i, size_t N) const
      {
        return min + i*(max-min)/(N-1);
      }

      
    protected:
        T min, max;
  };

  // using a factory function here so that argument types can be deduced.
  template<typename T,typename V>
  Uniform_imp<T> Uniform( T min, V max )
  {
    return Uniform_imp<T>(min,max);
  }





  template<typename T>
  class Geometric_imp
  {
    public:
      Geometric_imp( T min, T dx, T stretch )
      {
        this->min = min;
        this->dx = dx;
        this->stretch = stretch;
      }

      T operator()(size_t i, size_t N) const
      {
        // x[0] = xmin
        // x[1] = xmin + dx
        // x[2] = xmin + dx + s*dx
        // x[3] = xmin + dx + s*dx + s*s*dx
        // x[i] = xmin + dx*\sigma s^(i-1)
        return min + pow( dx, i-1 );
      }

      
    protected:
        T min, dx, stretch;
  };

  // using a factory function here so that argument types can be deduced.
  template<typename T,typename V>
  Geometric_imp<T> Geometric( T min, V max )
  {
    return Geometric_imp<T>(min,max);
  }

}


#endif // include protector
