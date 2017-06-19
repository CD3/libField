#include "catch.hpp"
#include <iostream>


#include "Field.hpp"



TEST_CASE( "Tutorial : 2D Temperature", "[tutorial]" ) {

  Field<double,2> T(10,15);
  
  T.setCoordinateSystem( Uniform(-3,3), Uniform(0,10) );
  T.set( 0.0 );

  CHECK( T.getCoord(1,1)[0] == Approx(-3   + 6./9) );
  CHECK( T.getCoord(1,1)[1] == Approx(10./14) );

  std::cout << T << std::endl;


  for(int i = 0; i < T.size(0); ++i)
  {
    for( int j = 0; j < T.size(1); j++)
    {
      auto x = T.getCoord(i,j);
      T(i,j) = exp( 2*( x[0]*x[0] + (x[1] - 5)*(x[1] - 5) ) );
    }
  }


  CHECK( T(5,5) > 0.1 ); // just make sure it got set to something.

  auto T2 = T.slice( indices[5][IRange()] );

  CHECK( T2(0) == T(5,0) );

  auto T3 = T.slice( indices[5][IRange(2,8)] );

  CHECK( T3(0) == T(5,2) );
  CHECK( T3(T3.size()-1) == T(5,7) );

  auto T4 = T.slice( indices[IRange(0,7,2)][1] );
  CHECK( T4(0) == T(0,1) );
  CHECK( T4(1) == T(2,1) );
  CHECK( T4(2) == T(4,1) );
  CHECK( T4(3) == T(6,1) );

}

