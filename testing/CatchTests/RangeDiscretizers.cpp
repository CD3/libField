#include "catch.hpp"

#include <libField/RangeDiscretizers.hpp>


TEST_CASE("Uniform Range Discretizer", "[ranges]")
{

  auto range = Uniform(1,11);

  CHECK(range(0,11) == Approx(1) );
  CHECK(range(1,11) == Approx(2) );
  CHECK(range(9,11) == Approx(10) );
  CHECK(range(10,11) == Approx(11) );

}

TEST_CASE("Geometric Range Discretizer", "[ranges]")
{

  auto range = Geometric(1.,0.1,2);

  CHECK(range(0,5) == Approx(1) );
  CHECK(range(1,5) == Approx(1.1) );
  CHECK(range(2,5) == Approx(1.3) );
  CHECK(range(3,5) == Approx(1.7) );
  CHECK(range(4,5) == Approx(2.5) );

}

TEST_CASE("Geometric with Period Range Discretizer", "[ranges]")
{

  auto range = GeometricWithPeriod(1.,0.1,2,1.);

  CHECK(range(0,10) == Approx(1) );
  CHECK(range(1,10) == Approx(1.1) );
  CHECK(range(2,10) == Approx(1.3) );
  CHECK(range(3,10) == Approx(1.7) );
  CHECK(range(4,10) == Approx(2.0) );
  CHECK(range(5,10) == Approx(2.1) );
  CHECK(range(6,10) == Approx(2.3) );
  CHECK(range(7,10) == Approx(2.7) );
  CHECK(range(8,10) == Approx(3.0) );

}
