#define CATCH_CONFIG_ENABLE_BENCHMARKING
#ifdef HAVE_GPUTILS
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <libField/Field.hpp>
#include <libField/IO.hpp>

#include "Utils.h"


TEST_CASE("ASCII Read and Write")
{
  SECTION("1D Fields")
  {
    SECTION("double out float in")
    {
      Field<double, 1> F(10);
      F.setCoordinateSystem(Uniform(0, 2));
      F.set_f([](auto x) { return x[0] * x[0]; });

      asciiwrite("1D-Field.txt", F);

      Field<float, 1> G;
      asciiread("1D-Field.txt", G);

      CHECK(G.size() == 10);
      CHECK(G.getCoord(0) == Catch::Approx(0));
      CHECK(G.getCoord(9) == Catch::Approx(2));
      CHECK(G(0) == Catch::Approx(0));
      CHECK(G(9) == Catch::Approx(4));
    }

    SECTION("double/float out float/double in")
    {
      Field<double, 1, float> F(10);
      F.setCoordinateSystem(Uniform(0, 2));
      F.set_f([](auto x) { return x[0] * x[0]; });

      asciiwrite("1D-Field.txt", F);

      Field<float, 1, double> G;
      asciiread("1D-Field.txt", G);

      CHECK(G.size() == 10);
      CHECK(G.getCoord(0) == Catch::Approx(0));
      CHECK(G.getCoord(9) == Catch::Approx(2));
      CHECK(G(0) == Catch::Approx(0));
      CHECK(G(9) == Catch::Approx(4));
    }

  }

  /* SECTION("2D Fields") */
  /* { */
  /*   SECTION("double out float in") */
  /*   { */
  /*     Field<double, 2> F(10, 20); */
  /*     F.setCoordinateSystem(Uniform(0, 2), Uniform(0, 4)); */
  /*     F.set_f([](auto x) { return x[0] * x[0] + x[1] * x[1]; }); */

  /*     asciiwrite("2D-Field.txt", F); */

  /*     Field<float, 2> G; */
  /*     asciiread("2D-Field.txt", G); */

  /*     CHECK(G.size() == 200); */
  /*     CHECK(G.size(0) == 10); */
  /*     CHECK(G.size(1) == 20); */
  /*     CHECK(G.getCoord(0, 0)[0] == Catch::Approx(0)); */
  /*     CHECK(G.getCoord(9, 0)[0] == Catch::Approx(2)); */
  /*     CHECK(G.getCoord(0, 19)[1] == Catch::Approx(4)); */
  /*     CHECK(G(0, 0) == Catch::Approx(0)); */
  /*     CHECK(G(9, 0) == Catch::Approx(4)); */
  /*     CHECK(G(0, 19) == Catch::Approx(16)); */
  /*     CHECK(G(9, 19) == Catch::Approx(20)); */
  /*   } */

  /*   SECTION("float out double in") */
  /*   { */
  /*     Field<float, 2> F(10, 20); */
  /*     F.setCoordinateSystem(Uniform(0, 2), Uniform(0, 4)); */
  /*     F.set_f([](auto x) { return x[0] * x[0] + x[1] * x[1]; }); */

  /*     asciiwrite("2D-Field.txt", F); */

  /*     Field<double, 2> G; */
  /*     asciiread("2D-Field.txt", G); */

  /*     CHECK(G.size() == 200); */
  /*     CHECK(G.size(0) == 10); */
  /*     CHECK(G.size(1) == 20); */
  /*     CHECK(G.getCoord(0, 0)[0] == Catch::Approx(0)); */
  /*     CHECK(G.getCoord(9, 0)[0] == Catch::Approx(2)); */
  /*     CHECK(G.getCoord(0, 19)[1] == Catch::Approx(4)); */
  /*     CHECK(G(0, 0) == Catch::Approx(0)); */
  /*     CHECK(G(9, 0) == Catch::Approx(4)); */
  /*     CHECK(G(0, 19) == Catch::Approx(16)); */
  /*     CHECK(G(9, 19) == Catch::Approx(20)); */
  /*   } */
  /* } */

}

#endif
