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

      T operator()(size_t i, size_t N)
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

}


#endif // include protector
