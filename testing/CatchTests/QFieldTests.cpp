#define CATCH_CONFIG_ENABLE_BENCHMARKING
#ifdef HAVE_HDF5_CPP
#include "catch.hpp"

#include <libField/Field.hpp>


TEST_CASE("QField Tests")
{
  SECTION("1D Field")
  {
    Field<double,1> F(10);
  }
}
#endif
