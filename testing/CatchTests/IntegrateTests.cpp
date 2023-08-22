#ifdef HAVE_LIBINTEGRATE

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <libField/Field.hpp>
#include <libIntegrate/Integrate.hpp>

TEST_CASE("libField/libIntegrate Integration")
{


  Field<double,1> f(100);
  f.setCoordinateSystem( Uniform(0.,M_PI) );
  f.set_f( [](auto x){return sin(x[0]);} );

  SECTION("Riemann Sum")
  {
    _1D::RiemannRule<double> integrate;
    CHECK( integrate(f.getAxis(0),f) == Catch::Approx(2.0).epsilon(0.0001) );
  }

  SECTION("Trapezoid Sum")
  {
    _1D::TrapezoidRule<double> integrate;
    CHECK( integrate(f.getAxis(0),f) == Catch::Approx(2.0).epsilon(0.0001) );
  }

  SECTION("Simpson's Sum")
  {
    _1D::SimpsonRule<double> integrate;
    CHECK( integrate(f.getAxis(0),f) == Catch::Approx(2.0).epsilon(0.0001) );
  }


}

#endif
