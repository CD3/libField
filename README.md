# libField

`libField` is a C++ library with a simple, clean interface for storing,
accessing, and manipulating fields (i.e. data with coordinates).
Its primary objective is to make writing programs that work with field data (such as
finite-difference based physics simulations) quick and simple.

Consider writing a 3-dimensional finite-difference heat solver. `libField` makes it simple
to create an array for the temperature and setup a coordinate system.

```
#include <Field.hpp>

int main()
{
  // create a 100x100x200 array to store the temperature over the domain x = [-5,5], y = [-5,5], z = [0,20].
  Field<double,3> T(100,100,200);
  T.setCoordinateSystem( Uniform(-5,5), Uniform(-5,5), Uniform(0,20) );

  // set initial temperature distribution to T(x,y,z) = x*y*z
  for(int i = 0; i < T.size(0); i++)
  {
    for(int j = 0; j < T.size(1); j++)
    {
      for(int k = 0; k < T.size(2); k++)
      {
        auto x = T.getCoord(i,j,k);
        T(i,j,k) = x[0]*x[1]*x[2];
      }
    }
  }
        
  // do work and profit

  
  return 0;
}

```
