#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <iostream>

#include <libField/Field.hpp>

TEST_CASE("Integer Rounding in Range Discretizers")
{
  // if we pass int literals to the range discretizers,
  // then we get integer rounding, which is bad.

  SECTION("Explicit Cast")
  {
  auto urange = Uniform<double>(0,2);
  auto grange = Geometric<double>(0,1,2);

  CHECK( urange(0,11) == Catch::Approx(0.0) );
  CHECK( urange(1,11) == Catch::Approx(0.2) );
  CHECK( urange(10,11) == Catch::Approx(2) );

  CHECK( grange(0,11) == Catch::Approx(0.0) );
  CHECK( grange(1,11) == Catch::Approx(1) );
  CHECK( grange(2,11) == Catch::Approx(3) );
  }

  SECTION("Argument Deduction")
  {
  // these now cause compiler error
  //auto urange = Uniform(0,2.5);
  //auto grange = Geometric(0,1.5,2);
  auto urange = Uniform(0.,2.5);
  auto grange = Geometric(0.,1.5,2);

  CHECK( urange(0,11) == Catch::Approx(0.0) );
  CHECK( urange(1,11) == Catch::Approx(0.25) );
  CHECK( urange(10,11) == Catch::Approx(2.5) );

  CHECK( grange(0,11) == Catch::Approx(0.0) );
  CHECK( grange(1,11) == Catch::Approx(1.5) );
  CHECK( grange(2,11) == Catch::Approx(4.5) );
  }
  
}
