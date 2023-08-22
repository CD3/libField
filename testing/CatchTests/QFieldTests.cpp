#ifdef HAVE_HDF5_CPP
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <libField/Field.hpp>

TEST_CASE("QField Tests")
{
  SECTION("1D Field")
  {
    Field<double, 1> F(10);
  }
}
#endif
