#if SERIALIZATION_ENABLED
#include "catch.hpp"
#include "fakeit.hpp"

#include<fstream>
#include<sstream>
#include<boost/archive/text_oarchive.hpp>
#include<boost/archive/text_iarchive.hpp>

#include "Utils.h"

#include "CoordinateSystem.hpp"
#include "Field.hpp"
#include "Serialization.hpp"

using namespace boost;

TEST_CASE( "MultiArray Serialization - 1D" ) {

  std::stringstream ss;

  boost::multi_array<double,1> a(extents[10]), b;

  for(int i = 0; i < 10; i++)
  {
    a[i] = 10*i;
  }

  boost::archive::text_oarchive oa(ss);
  oa << a;
  boost::archive::text_iarchive ia(ss);
  ia >> b;

  for( int i = 0; i < 10; i++)
  {
    CHECK( b[i] == a[i] );
  }

}

TEST_CASE( "MultiArray Serialization - 2D" ) {

  std::stringstream ss;


  SECTION( "Vanilla" ) {

    boost::multi_array<double,2> a(extents[10][20]), b;

    for(int i = 0; i < 10; i++)
    {
      for(int j = 0; j < 20; j++)
      {
        a[i][j] = 2*i + 2*j;
      }
    }

    boost::archive::text_oarchive oa(ss);
    oa << a;
    boost::archive::text_iarchive ia(ss);
    ia >> b;

    for( int i = 0; i < 10; i++)
    {
      for(int j = 0; j < 20; j++)
      {
        CHECK( b[i][j] == a[i][j] );
      }
    }

  }

  SECTION( "With Index Offsets" ) {

    boost::multi_array<double,2> a(extents[10][20]), b;

    for(int i = 0; i < 10; i++)
    {
      for(int j = 0; j < 20; j++)
      {
        a[i][j] = 10*i + j;
      }
    }

    a.reindex( array<int,2>( {1,2} ) );

    boost::archive::text_oarchive oa(ss);
    oa << a;
    boost::archive::text_iarchive ia(ss);
    ia >> b;

    for( int i = 0+1; i < 10+1; i++)
    {
      for(int j = 0+2; j < 20+2; j++)
      {
        CHECK( b[i][j] == a[i][j] );
      }
    }

  }

  SECTION( "With Fortran Storage" ) {

    boost::multi_array<double,2> a(extents[10][20], fortran_storage_order()), b;

    for(int i = 0; i < 10; i++)
    {
      for(int j = 0; j < 20; j++)
      {
        a[i][j] = 10*i + j;
      }
    }

    boost::archive::text_oarchive oa(ss);
    oa << a;
    boost::archive::text_iarchive ia(ss);
    ia >> b;

    for( int i = 0; i < 10; i++)
    {
      for(int j = 0; j < 20; j++)
      {
        CHECK( b[i][j] == a[i][j] );
      }
    }

  }

  SECTION( "With Fortran Layout and Index Offsets" ) {

    // not working yet
    // cannot find a good way to serialize the storage order
    boost::multi_array<double,2> a(extents[10][20], fortran_storage_order()), b;
    //boost::multi_array<double,2> a(extents[10][20]), b;

    for(int i = 0; i < 10; i++)
    {
      for(int j = 0; j < 20; j++)
      {
        a[i][j] = 10*i + j;
      }
    }

    a.reindex( array<int,2>( {1,2} ) );

    boost::archive::text_oarchive oa(ss);
    oa << a;
    boost::archive::text_iarchive ia(ss);
    ia >> b;

    for( int i = 0+1; i < 10+1; i++)
    {
      for(int j = 0+2; j < 20+2; j++)
      {
        CHECK( b[i][j] == a[i][j] );
      }
    }

  }

}

TEST_CASE( "MultiArray Serialization - 3D" ) {

  boost::multi_array<double,3> a(extents[10][20][30]), b;

  for(int i = 0; i < 10; i++)
  {
    for(int j = 0; j < 20; j++)
    {
      for(int k = 0; k < 30; k++)
      {
        a[i][j][k] = 2*i + 2*j + 2*k;
      }
    }
  }

  std::stringstream ss;
  boost::archive::text_oarchive oa(ss);
  oa << a;

  boost::archive::text_iarchive ia(ss);
  ia >> b;

  for( int i = 0; i < 10; i++)
  {
    for(int j = 0; j < 20; j++)
    {
      for(int k = 0; k < 30; k++)
      {
        CHECK( b[i][j][k] == a[i][j][k] );
      }
    }
  }

}




TEST_CASE( "CoordinateSystem Serialization" ) {

  CoordinateSystem<double,3> Coordinates(11,11,11);
  Coordinates.set( Uniform(0,10), Uniform(10,20), Uniform(20,30) );

  for(int i = 0; i < 10; i++)
    CHECK( Coordinates[0][i] == 0 + i );
  for(int i = 0; i < 10; i++)
    CHECK( Coordinates[1][i] == 10 + i );
  for(int i = 0; i < 10; i++)
    CHECK( Coordinates[2][i] == 20 + i );


  std::ofstream out("coordsys-ar.txt");
  boost::archive::text_oarchive oa(out);
  oa << Coordinates;
  out.close();

  CoordinateSystem<double,3> Coordinates2;

  std::ifstream in("coordsys-ar.txt");
  boost::archive::text_iarchive ia(in);
  ia >> Coordinates2;
  in.close();


  for(int i = 0; i < 10; i++)
    CHECK( Coordinates2[0][i] == 0 + i );
  for(int i = 0; i < 10; i++)
    CHECK( Coordinates2[1][i] == 10 + i );
  for(int i = 0; i < 10; i++)
    CHECK( Coordinates2[2][i] == 20 + i );


}

TEST_CASE( "Field Serialization" ) {

  int Nx = 11, Ny = 6, Nz = 21;
  Field<double,3> T( Nx, Ny, Nz );
  T.getCoordinateSystem().set( Uniform(0.,10.), Uniform(0.,10.), Uniform(0.,10.) );

  for( int i = 0; i < Nx; ++i )
  {
    for( int j = 0; j < Ny; ++j )
    {
      for( int k = 0; k < Nz; ++k )
      {
        auto x = T.getCoord(i,j,k);
        T[i][j][k] = x[0]*x[1]*x[2];
      }
    }
  }

  std::ofstream out("field-ar.txt");
  boost::archive::text_oarchive oa(out);
  oa << T;
  out.close();

  Field<double,3> T2( Nx, Ny, Nz );

  std::ifstream in("field-ar.txt");
  boost::archive::text_iarchive ia(in);
  ia >> T2;
  in.close();


  for( int i = 0; i < Nx; ++i )
  {
    for( int j = 0; j < Ny; ++j )
    {
      for( int k = 0; k < Nz; ++k )
      {
        auto X = T2.getCoord(i,j,k);
        CHECK( X[0] == Approx(i*1.0) );
        CHECK( X[1] == Approx(j*2.0) );
        CHECK( X[2] == Approx(k*0.5) );

        auto F = T2.getCoordinateSystem()[0][i]
               * T2.getCoordinateSystem()[1][j]
               * T2.getCoordinateSystem()[2][k];

        // this is slow, but looks nice
        CHECK( T2(i,j,k) == Approx(F) );
      }
    }
  }


}
#endif
