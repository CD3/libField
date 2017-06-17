#include "catch.hpp"
#include "fakeit.hpp"

// This is a template unit test file. To create a new unit tests, copy this file
// and add your tests inside the TEST_CASE function.

TEST_CASE( "Template Catch Test", "[template]" ) {
  REQUIRE( 1.23 == Approx( 1.23 ) );

  SECTION( "Template Catch Test Section 1") {
    REQUIRE( 2.34 == Approx( 2.34 ) );
  }

  SECTION( "Template Catch Test Section 2") {
    REQUIRE( 3.4 == Approx( 3.4 ) );
  }
}

