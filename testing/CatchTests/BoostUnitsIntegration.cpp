#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include<iostream>
#include<libField/Field.hpp>
#include<boost/units/systems/si.hpp>
#include<boost/units/quantity.hpp>
#include<boost/units/io.hpp>

using second = boost::units::si::time;
using meter = boost::units::si::length;
template<typename U>
using q = boost::units::quantity<U>;

TEST_CASE("Using Boost.Units")
{

  Field<q<meter>,1,q<second>> x_vs_t(3);


  x_vs_t.getAxis(0)[0] = 0.10*second();
  x_vs_t.getAxis(0)[1] = 0.20*second();
  x_vs_t.getAxis(0)[2] = 0.30*second();

  x_vs_t(0) = 10*meter();
  x_vs_t(1) = 20*meter();
  x_vs_t(2) = 30*meter();

  // this will print
  // 0.1 s 10 m
  // 0.2 s 20 m
  // 0.3 s 30 m
  //std::cout << "x_vs_t: " << x_vs_t << std::endl;



}

