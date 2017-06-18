#include <Field.hpp>

int main()
{
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
        
  // do work

  
  return 0;
}
