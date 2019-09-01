#define CATCH_CONFIG_ENABLE_BENCHMARKING
#ifdef HAVE_HDF5_CPP
#include "catch.hpp"

#include <libField/Field.hpp>
#include <libField/HDF5.hpp>

#include "Utils.h"

TEST_CASE("HDF5 Read and Write")
{
  SECTION("1D Fields")
  {
    SECTION("double out float in")
    {
      Field<double, 1> F(10);
      F.setCoordinateSystem(Uniform(0, 2));
      F.set_f([](auto x) { return x[0] * x[0]; });

      hdf5write( "1D-Field.h5", F );

      Field<float,1> G;
      hdf5read("1D-Field.h5", G);

      CHECK( G.size() == 10 );
      CHECK( G.getCoord(0) == Approx(0) );
      CHECK( G.getCoord(9) == Approx(2) );
      CHECK( G(0) == Approx(0) );
      CHECK( G(9) == Approx(4) );
    }

    SECTION("double/float out float/double in")
    {
      Field<double, 1, float> F(10);
      F.setCoordinateSystem(Uniform(0, 2));
      F.set_f([](auto x) { return x[0] * x[0]; });

      hdf5write( "1D-Field.h5", F );

      Field<float,1, double> G;
      hdf5read("1D-Field.h5", G);

      CHECK( G.size() == 10 );
      CHECK( G.getCoord(0) == Approx(0) );
      CHECK( G.getCoord(9) == Approx(2) );
      CHECK( G(0) == Approx(0) );
      CHECK( G(9) == Approx(4) );
    }

    SECTION("writing to groups")
    {
      Field<double, 1> F(10);
      F.setCoordinateSystem(Uniform(0, 2));
      F.set_f([](auto x) { return x[0] * x[0]; });

      //hdf5write( "1D-Field.h5", "/electric field", F );
      //hdf5write( "1D-Field.h5", "/temperature", F );

    }
    SECTION("Mismatched dimensions throws")
    {
      Field<double, 1> F(10);
      F.setCoordinateSystem(Uniform(0, 2));
      F.set(0.0);

      hdf5write( "1D-Field.h5", F );

      Field<float,2> G;
      CHECK_THROWS( hdf5read("1D-Field.h5", G) );
    }

  }




  SECTION("2D Fields")
  {
    SECTION("double out float in")
    {
      Field<double, 2> F(10,20);
      F.setCoordinateSystem(Uniform(0,2),Uniform(0,4));
      F.set_f([](auto x) { return x[0] * x[0] + x[1]*x[1]; });

      hdf5write( "2D-Field.h5", F );

      Field<float,2> G;
      hdf5read("2D-Field.h5", G);

      CHECK( G.size() == 200 );
      CHECK( G.size(0) == 10 );
      CHECK( G.size(1) == 20 );
      CHECK( G.getCoord(0,0)[0] == Approx(0) );
      CHECK( G.getCoord(9,0)[0] == Approx(2) );
      CHECK( G.getCoord(0,19)[1] == Approx(4) );
      CHECK( G(0,0) == Approx(0) );
      CHECK( G(9,0) == Approx(4) );
      CHECK( G(0,19) == Approx(16) );
      CHECK( G(9,19) == Approx(20) );
    }

    SECTION("float out double in")
    {
      Field<float, 2> F(10,20);
      F.setCoordinateSystem(Uniform(0,2),Uniform(0,4));
      F.set_f([](auto x) { return x[0] * x[0] + x[1]*x[1]; });

      hdf5write( "2D-Field.h5", F );

      Field<double,2> G;
      hdf5read("2D-Field.h5", G);

      CHECK( G.size() == 200 );
      CHECK( G.size(0) == 10 );
      CHECK( G.size(1) == 20 );
      CHECK( G.getCoord(0,0)[0] == Approx(0) );
      CHECK( G.getCoord(9,0)[0] == Approx(2) );
      CHECK( G.getCoord(0,19)[1] == Approx(4) );
      CHECK( G(0,0) == Approx(0) );
      CHECK( G(9,0) == Approx(4) );
      CHECK( G(0,19) == Approx(16) );
      CHECK( G(9,19) == Approx(20) );
    }




    
  }
}
#endif
