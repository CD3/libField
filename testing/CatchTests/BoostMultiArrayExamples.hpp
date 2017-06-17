#include "catch.hpp"
#include "fakeit.hpp"

#include<vector>

#include<boost/multi_array.hpp>

using namespace boost;

template<typename T, long unsigned int N>
using ma = multi_array<T,N>;
template<typename T, long unsigned int N>
using I = typename ma<T,N>::index;
typedef multi_array_types::index_range R;

TEST_CASE( "Boost.Multiarray Examples", "[example]" ) {

  int Nx = 2, Ny = 3, Nz = 4;
  ma<double,3> cube(extents[2][3][4]);

  for(int i = 0; i < Nx; ++i)
    for(int j = 0; j < Ny; ++j)
      for(int k = 0; k < Nz; ++k)
        cube[i][j][k] = i*j*k;

  for(int i = 0; i < Nx; ++i)
    for(int j = 0; j < Ny; ++j)
      for(int k = 0; k < Nz; ++k)
        CHECK( cube[i][j][k] == Approx(i*j*k) );

  auto plane = cube[indices[R()][1][R()]];

  for(int i = 0; i < Nx; ++i)
    for(int k = 0; k < Nz; ++k)
      CHECK( plane[i][k] == Approx(i*1*k) );

  plane[0][0] = -10;

  CHECK( plane[0][0] == Approx(-10) );
  CHECK( cube[0][0][0] == Approx(0) );
  CHECK( cube[0][1][0] == Approx(-10) );

  cube[0][1][0] = 0*1*0;

  auto line = plane[indices[1][R()]];

  CHECK( line[0] == plane[1][0] );
  CHECK( line[0] == cube[1][1][0] );

  line[2] = -10;

  CHECK( line[2] == -10 );
  CHECK( plane[1][2] == -10 );
  CHECK( cube[1][1][2] == -10 );

}


TEST_CASE( "Boost.Multiarray Complex Matrix", "[example]" ) {

  int N = 3;

  ma<double,3> A(extents[N][N][2]);

  A[0][0][0] = 1; A[0][0][1] = 2;
  A[0][1][0] = 1; A[0][1][1] = 2;
  A[0][2][0] = 1; A[0][2][1] = 2;
  A[1][0][0] = 1; A[1][0][1] = 2;
  A[1][1][0] = 1; A[1][1][1] = 2;
  A[1][2][0] = 1; A[1][2][1] = 2;
  A[2][0][0] = 1; A[2][0][1] = 2;
  A[2][1][0] = 1; A[2][1][1] = 2;
  A[2][2][0] = 1; A[2][2][1] = 2;


  auto Ar = A[ indices[R()][R()][0] ];
  auto Ai = A[ indices[R()][R()][1] ];

  for( int i = 0; i < N; ++i )
    for( int j = 0; j < N; ++j )
      CHECK( Ar[i][j] == 1 );

  for( int i = 0; i < N; ++i )
    for( int j = 0; j < N; ++j )
      CHECK( Ai[i][j] == 2 );

}



