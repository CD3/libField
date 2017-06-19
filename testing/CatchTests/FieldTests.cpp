#include "catch.hpp"
#include "fakeit.hpp"

#include "Utils.h"

#include "Field.hpp"



TEST_CASE( "Field Usage" ) {

  int Nx = 11, Ny = 6, Nz = 21;
  Field<double,3> Temperature( Nx, Ny, Nz );
  Temperature.getCoordinateSystem().set( Uniform(0.,10.), Uniform(0.,10.), Uniform(0.,10.) );
  Field<double,3> Pressure( Temperature.getCoordinateSystemPtr() );


  for( int i = 0; i < Nx; ++i )
  {
    for( int j = 0; j < Ny; ++j )
    {
      for( int k = 0; k < Nz; ++k )
      {
        auto x = Temperature.getCoord(i,j,k);
        Temperature[i][j][k] = x[0]*x[1]*x[2];
      }
    }
  }

  Pressure.set( [](auto i, auto cs){
        auto x = cs->getCoord(i[0],i[1],i[2]);
        return 2*x[0]*x[1]*x[2];
      });

  CHECK( Temperature.getCoordinateSystem()[2][0] == Approx(0.0) );
  CHECK( Temperature.getCoordinateSystem()[2][1] == Approx(0.5) );
  CHECK( Temperature.getCoordinateSystem()[2][2] == Approx(1.0) );
  CHECK( Temperature.getCoordinateSystem()[2][3] == Approx(1.5) );
  for( int i = 0; i < Nx; ++i )
  {
    for( int j = 0; j < Ny; ++j )
    {
      for( int k = 0; k < Nz; ++k )
      {
        CHECK( Temperature.getCoordinateSystem()[0][i] == Approx(i*1.0) );
        CHECK( Temperature.getCoordinateSystem()[1][j] == Approx(j*2.0) );
        CHECK( Temperature.getCoordinateSystem()[2][k] == Approx(k*0.5) );

        CHECK( Pressure.getCoordinateSystem()[0][i] == Approx(i*1.0) );
        CHECK( Pressure.getCoordinateSystem()[1][j] == Approx(j*2.0) );
        CHECK( Pressure.getCoordinateSystem()[2][k] == Approx(k*0.5) );

        auto X = Temperature.getCoordinateSystem().getCoord(i,j,k);
        CHECK( X[0] == Approx(i*1.0) );
        CHECK( X[1] == Approx(j*2.0) );
        CHECK( X[2] == Approx(k*0.5) );

        auto XX = Temperature.getCoord(i,j,k);
        CHECK( XX[0] == Approx(i*1.0) );
        CHECK( XX[1] == Approx(j*2.0) );
        CHECK( XX[2] == Approx(k*0.5) );

        auto F = Temperature.getCoordinateSystem()[0][i]
               * Temperature.getCoordinateSystem()[1][j]
               * Temperature.getCoordinateSystem()[2][k];

        // this is slow, but looks nice
        CHECK( Temperature(i,j,k) == Approx(F) );
        // this is faster
        CHECK( Temperature(std::vector<int>({i,j,k})) == Approx(F) );
        // this is fastest
        CHECK( Temperature[i][j][k] == Approx(F) );

        // can use the underlying multi_array
        CHECK( Temperature.getData()[i][j][k] == Approx(F) );
        // or raw pointer
        CHECK( Temperature.data()[ i*Ny*Nz + j*Nz + k ] == Approx(F) );


      }
    }
  }


  // Pressure field is separate from Temperature
  for( int i = 0; i < Nx; ++i )
  {
    for( int j = 0; j < Ny; ++j )
    {
      for( int k = 0; k < Nz; ++k )
      {
        auto X = Pressure.getCoord(i,j,k);
        CHECK( Pressure[i][j][k] == Approx(2*X[0]*X[1]*X[2]) );
      }
    }
  }

  // but, they share a coorinate system
  CHECK( Temperature.getCoordinateSystem()[0][0] != -1 );
  CHECK( Pressure.getCoordinateSystem()[0][0] != -1 );

  Temperature.getCoordinateSystem()[0][0] = -1;
  CHECK( Temperature.getCoordinateSystem()[0][0] == -1 );
  CHECK( Pressure.getCoordinateSystem()[0][0] == -1 );

  Pressure.getCoordinateSystem()[0][0] = -2;
  CHECK( Temperature.getCoordinateSystem()[0][0] == -2 );
  CHECK( Pressure.getCoordinateSystem()[0][0] == -2 );



}


TEST_CASE("Field Slicing")
{
  int Nx = 6, Ny = 6, Nz = 6;
  Field<double,3> F1( Nx, Ny, Nz );
  F1.getCoordinateSystem().set( Uniform(0.,5.), Uniform(0.,5.), Uniform(0.,5.) );

  for( int i = 0; i < Nx; ++i )
  {
    for( int j = 0; j < Ny; ++j )
    {
      for( int k = 0; k < Nz; ++k )
      {
        F1[i][j][k] = i*j*k;
      }
    }
  }

  auto F2 = F1.slice( indices[IRange()][2][IRange(1,6,2)] );

  CHECK( F2(0,0) == F1(0,2,1) );
  CHECK( F2(0,1) == F1(0,2,3) );
  CHECK( F2(0,2) == F1(0,2,5) );

  CHECK( F2(1,0) == F1(1,2,1) );
  CHECK( F2(1,1) == F1(1,2,3) );
  CHECK( F2(1,2) == F1(1,2,5) );

  CHECK( F2(2,0) == F1(2,2,1) );
  CHECK( F2(2,1) == F1(2,2,3) );
  CHECK( F2(2,2) == F1(2,2,5) );
  
  CHECK( F2(3,0) == F1(3,2,1) );
  CHECK( F2(3,1) == F1(3,2,3) );
  CHECK( F2(3,2) == F1(3,2,5) );
  
  CHECK( F2(4,0) == F1(4,2,1) );
  CHECK( F2(4,1) == F1(4,2,3) );
  CHECK( F2(4,2) == F1(4,2,5) );

  CHECK( F2(5,0) == F1(5,2,1) );
  CHECK( F2(5,1) == F1(5,2,3) );
  CHECK( F2(5,2) == F1(5,2,5) );

  CHECK( F2.getAxis(1)[0] == 1 );
  CHECK( F2.getAxis(1)[1] == 3 );
  CHECK( F2.getAxis(1)[2] == 5 );


}

