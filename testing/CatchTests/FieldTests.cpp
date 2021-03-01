#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"

#include <libField/Field.hpp>
#include "Utils.h"
#include "fakeit.hpp"

TEST_CASE("Field Usage")
{
  int              Nx = 11, Ny = 6, Nz = 21;
  Field<double, 3> Temperature(Nx, Ny, Nz);
  Temperature.getCoordinateSystem().set(Uniform(0., 10.), Uniform(0., 10.),
                                        Uniform(0., 10.));
  Field<double, 3> Pressure(Temperature.getCoordinateSystemPtr());

  auto ind = Temperature.lower_bound(5, 5, 5);
  CHECK(ind[0] == 5);
  CHECK(ind[1] == 2);
  CHECK(ind[2] == 10);

  ind = Temperature.upper_bound(5, 5, 5);
  CHECK(ind[0] == 6);
  CHECK(ind[1] == 3);
  CHECK(ind[2] == 11);

  for (int i = 0; i < Nx; ++i) {
    for (int j = 0; j < Ny; ++j) {
      for (int k = 0; k < Nz; ++k) {
        auto x               = Temperature.getCoord(i, j, k);
        Temperature[i][j][k] = x[0] * x[1] * x[2];
      }
    }
  }

  Pressure.set_f([](auto i, auto cs) {
    auto x = cs->getCoord(i[0], i[1], i[2]);
    return 2 * x[0] * x[1] * x[2];
  });

  CHECK(Temperature.getCoordinateSystem()[2][0] == Approx(0.0));
  CHECK(Temperature.getCoordinateSystem()[2][1] == Approx(0.5));
  CHECK(Temperature.getCoordinateSystem()[2][2] == Approx(1.0));
  CHECK(Temperature.getCoordinateSystem()[2][3] == Approx(1.5));
  for (int i = 0; i < Nx; ++i) {
    for (int j = 0; j < Ny; ++j) {
      for (int k = 0; k < Nz; ++k) {
        CHECK(Temperature.getCoordinateSystem()[0][i] == Approx(i * 1.0));
        CHECK(Temperature.getCoordinateSystem()[1][j] == Approx(j * 2.0));
        CHECK(Temperature.getCoordinateSystem()[2][k] == Approx(k * 0.5));

        CHECK(Pressure.getCoordinateSystem()[0][i] == Approx(i * 1.0));
        CHECK(Pressure.getCoordinateSystem()[1][j] == Approx(j * 2.0));
        CHECK(Pressure.getCoordinateSystem()[2][k] == Approx(k * 0.5));

        auto X = Temperature.getCoordinateSystem().getCoord(i, j, k);
        CHECK(X[0] == Approx(i * 1.0));
        CHECK(X[1] == Approx(j * 2.0));
        CHECK(X[2] == Approx(k * 0.5));

        auto XX = Temperature.getCoord(i, j, k);
        CHECK(XX[0] == Approx(i * 1.0));
        CHECK(XX[1] == Approx(j * 2.0));
        CHECK(XX[2] == Approx(k * 0.5));

        auto F = Temperature.getCoordinateSystem()[0][i] *
                 Temperature.getCoordinateSystem()[1][j] *
                 Temperature.getCoordinateSystem()[2][k];

        // this is slow, but looks nice
        CHECK(Temperature(i, j, k) == Approx(F));
        // this is faster
        CHECK(Temperature(std::vector<int>({i, j, k})) == Approx(F));
        // this is fastest
        CHECK(Temperature[i][j][k] == Approx(F));

        // can use the underlying multi_array
        CHECK(Temperature.getData()[i][j][k] == Approx(F));
        // or raw pointer
        CHECK(Temperature.data()[i * Ny * Nz + j * Nz + k] == Approx(F));
      }
    }
  }

  // Pressure field is separate from Temperature
  for (int i = 0; i < Nx; ++i) {
    for (int j = 0; j < Ny; ++j) {
      for (int k = 0; k < Nz; ++k) {
        auto X = Pressure.getCoord(i, j, k);
        CHECK(Pressure[i][j][k] == Approx(2 * X[0] * X[1] * X[2]));
      }
    }
  }

  // but, they share a coorinate system
  CHECK(Temperature.getCoordinateSystem()[0][0] != -1);
  CHECK(Pressure.getCoordinateSystem()[0][0] != -1);

  Temperature.getCoordinateSystem()[0][0] = -1;
  CHECK(Temperature.getCoordinateSystem()[0][0] == -1);
  CHECK(Pressure.getCoordinateSystem()[0][0] == -1);

  Pressure.getCoordinateSystem()[0][0] = -2;
  CHECK(Temperature.getCoordinateSystem()[0][0] == -2);
  CHECK(Pressure.getCoordinateSystem()[0][0] == -2);
}

TEST_CASE("Field Operators")
{
  Field<double, 2> T(2, 3), U(2, 3);

  T = 1.0;
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++) CHECK(T[i][j] == Approx(1.0));

  T += 2.0;
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++) CHECK(T[i][j] == Approx(3.0));

  T -= 1.0;
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++) CHECK(T[i][j] == Approx(2.0));

  T *= 3.0;
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++) CHECK(T[i][j] == Approx(6.0));

  T /= 2.0;
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++) CHECK(T[i][j] == Approx(3.0));

  U = T;
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++) CHECK(U[i][j] == Approx(3.0));
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++) CHECK(T[i][j] == Approx(3.0));

  U += T;
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++) CHECK(U[i][j] == Approx(6.0));
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++) CHECK(T[i][j] == Approx(3.0));

  U *= T;
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++) CHECK(U[i][j] == Approx(18.0));
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++) CHECK(T[i][j] == Approx(3.0));

  U -= T;
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++) CHECK(U[i][j] == Approx(15.0));
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++) CHECK(T[i][j] == Approx(3.0));

  U /= T;
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++) CHECK(U[i][j] == Approx(5.0));
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++) CHECK(T[i][j] == Approx(3.0));
}

TEST_CASE("Field Slicing")
{
  int              Nx = 6, Ny = 6, Nz = 6;
  Field<double, 3> F1(Nx, Ny, Nz);
  F1.getCoordinateSystem().set(Uniform(0., 5.), Uniform(0., 5.),
                               Uniform(0., 5.));

  for (int i = 0; i < Nx; ++i) {
    for (int j = 0; j < Ny; ++j) {
      for (int k = 0; k < Nz; ++k) {
        F1[i][j][k] = i * j * k;
      }
    }
  }

  auto F2 = F1.slice(indices[IRange()][2][IRange(1, 6, 2)]);

  CHECK(F2(0, 0) == F1(0, 2, 1));
  CHECK(F2(0, 1) == F1(0, 2, 3));
  CHECK(F2(0, 2) == F1(0, 2, 5));

  CHECK(F2(1, 0) == F1(1, 2, 1));
  CHECK(F2(1, 1) == F1(1, 2, 3));
  CHECK(F2(1, 2) == F1(1, 2, 5));

  CHECK(F2(2, 0) == F1(2, 2, 1));
  CHECK(F2(2, 1) == F1(2, 2, 3));
  CHECK(F2(2, 2) == F1(2, 2, 5));

  CHECK(F2(3, 0) == F1(3, 2, 1));
  CHECK(F2(3, 1) == F1(3, 2, 3));
  CHECK(F2(3, 2) == F1(3, 2, 5));

  CHECK(F2(4, 0) == F1(4, 2, 1));
  CHECK(F2(4, 1) == F1(4, 2, 3));
  CHECK(F2(4, 2) == F1(4, 2, 5));

  CHECK(F2(5, 0) == F1(5, 2, 1));
  CHECK(F2(5, 1) == F1(5, 2, 3));
  CHECK(F2(5, 2) == F1(5, 2, 5));

  CHECK(F2.getAxis(1)[0] == 1);
  CHECK(F2.getAxis(1)[1] == 3);
  CHECK(F2.getAxis(1)[2] == 5);
}

TEST_CASE("Field Output Operator")
{
  std::stringstream ss;
  std::string       expected;

  Field<double, 1> F1D(3);
  F1D.getCoordinateSystem().set(Uniform(0., 10.));
  F1D.set_f([](auto i, auto cs) { return 2 * i[0]; });

  ss << F1D;
  expected = R"(0 0
5 2
10 4
)";

  CHECK(ss.str() == expected);

  Field<double, 2> F2D(3, 3);
  F2D.getCoordinateSystem().set(Uniform(0., 10.), Uniform(-5., 5.));
  F2D.set_f([](auto i, auto cs) { return 2 * i[0] * i[1]; });

  ss.str("");
  ss << F2D;
  expected = R"(0 -5 0
0 0 0
0 5 0

5 -5 0
5 0 2
5 5 4

10 -5 0
10 0 4
10 5 8
)";

  CHECK(ss.str() == expected);

  Field<double, 3> F3D(3, 3, 3);
  F3D.getCoordinateSystem().set(Uniform(0., 10.), Uniform(-5., 5.),
                                Uniform(10, 20));
  F3D.set_f([](auto i, auto cs) { return 2 * i[0] * i[1] * i[2]; });

  ss.str("");
  ss << F3D;
  expected = R"(0 -5 10 0
0 -5 15 0
0 -5 20 0

0 0 10 0
0 0 15 0
0 0 20 0

0 5 10 0
0 5 15 0
0 5 20 0


5 -5 10 0
5 -5 15 0
5 -5 20 0

5 0 10 0
5 0 15 2
5 0 20 4

5 5 10 0
5 5 15 4
5 5 20 8


10 -5 10 0
10 -5 15 0
10 -5 20 0

10 0 10 0
10 0 15 4
10 0 20 8

10 5 10 0
10 5 15 8
10 5 20 16
)";

  CHECK(ss.str() == expected);
}

TEST_CASE("Field Semantics")
{
  SECTION("1d")
  {
    Field<double, 1> a(10);
    for (int i = 0; i < 10; ++i) a(i) = 0.1 * i + 2;
    a.setCoordinateSystem(Uniform(0, 1));

    SECTION("Is copy constructible")
    {
      Field<double, 1> b(a);
      CHECK(b.size() == 10);
      for (int i = 0; i < 10; ++i) CHECK(b(i) == Approx(2 + i * 0.1));
      CHECK(b.getAxis(0)[0] == Approx(0));
      CHECK(b.getAxis(0)[9] == Approx(1));
    }

    SECTION("Is move constructible from var with std::move()")
    {
      Field<double, 1> b(std::move(a));
      CHECK(b.size() == 10);
      for (int i = 0; i < 10; ++i) CHECK(b(i) == Approx(2 + i * 0.1));
      CHECK(b.getAxis(0)[0] == Approx(0));
      CHECK(b.getAxis(0)[9] == Approx(1));

      // the data and coordinate system pointers of a should be invalid now
      CHECK(b.getDataPtr());
      CHECK(b.getCoordinateSystemPtr());
      CHECK(!a.getDataPtr());
      CHECK(!a.getCoordinateSystemPtr());
    }

    SECTION("Is copy construct assignable")
    {
      Field<double, 1> b = a;
      CHECK(b.size() == 10);
      for (int i = 0; i < 10; ++i) CHECK(b(i) == Approx(2 + i * 0.1));
    }

    SECTION("Is assignable")
    {
      Field<double, 1> b;
      b = a;
      CHECK(b.size() == 10);
      for (int i = 0; i < 10; ++i) CHECK(b(i) == Approx(2 + i * 0.1));

      SECTION("and not linked")
      {
        b(0) = 100;
        CHECK(b(0) == Approx(100));
        CHECK(a(0) == Approx(2));

        b.getAxis(0)[9] = 100;
        CHECK(b.getAxis(0)[9] == Approx(100));
        CHECK(a.getAxis(0)[9] == Approx(1));
      }
    }
  }

  SECTION("2d")
  {
    Field<double, 2> a(10, 20);
    for (int i = 0; i < 10; ++i) {
      for (int j = 0; j < 20; ++j) {
        a(i, j) = 0.2 * j + 0.1 * i + 2;
      }
    }

    SECTION("Is copy constructible")
    {
      Field<double, 2> b(a);
      CHECK(b.size(0) == 10);
      CHECK(b.size(1) == 20);
      CHECK(b.size() == 200);
      for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 20; ++j) {
          CHECK(b(i, j) == Approx(2 + i * 0.1 + j * 0.2));
        }
      }
    }

    SECTION("Is copy construct assignable")
    {
      Field<double, 2> b = a;
      CHECK(b.size(0) == 10);
      CHECK(b.size(1) == 20);
      CHECK(b.size() == 200);
      for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 20; ++j) {
          CHECK(b(i, j) == Approx(2 + i * 0.1 + j * 0.2));
        }
      }
    }

    SECTION("Is assignable")
    {
      Field<double, 2> b;
      b = a;
      CHECK(b.size(0) == 10);
      CHECK(b.size(1) == 20);
      CHECK(b.size() == 200);
      for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 20; ++j) {
          CHECK(b(i, j) == Approx(2 + i * 0.1 + j * 0.2));
        }
      }
    }
  }
}

TEST_CASE("Field Copy vs. Move","[.][benchmarks]")
{
  Field<double, 3> F1(100, 100, 100);

  BENCHMARK("Field Copy") { Field<double, 3> F2(F1); };

  BENCHMARK("Field Move") { Field<double, 3> F2(std::move(F1)); };
}

#include <boost/optional.hpp>
TEST_CASE("Field::set_f")
{
  SECTION("1D")
  {
    Field<double, 1> F(11);
    F.setCoordinateSystem(Uniform(0, 10));

    CHECK(F.getCoord(0) == Approx(0));
    CHECK(F.getCoord(10) == Approx(10));

    SECTION("1 arg signature")
    {
      F.set_f([](auto x) { return 2 * x[0] + 4; });

      CHECK(F(0) == Approx(4));
      CHECK(F(10) == Approx(24));
    }

    SECTION("1 arg signature with optional return")
    {
      F.set(-1);
      F.set_f([](auto x) -> boost::optional<double> {
        if (x[0] < 5.5) return 2 * x[0] + 4;
        return boost::none;
      });

      CHECK(F(0) == Approx(4));
      CHECK(F(5) == Approx(14));
      CHECK(F(6) == Approx(-1));
      CHECK(F(10) == Approx(-1));
    }

    SECTION("2 arg signature")
    {
      F.set_f([](auto ind, auto cs) {
        auto x = cs->getCoord(ind);
        return 2 * x[0] + 3;
      });

      CHECK(F(0) == Approx(3));
      CHECK(F(10) == Approx(23));
    }

    SECTION("2 arg signature with optional return")
    {
      F.set(-1);
      F.set_f([](auto ind, auto cs) -> boost::optional<double> {
        if (ind[0] <= 4) {
          auto x = cs->getCoord(ind);
          return 2 * x[0] + 3;
        }
        return boost::none;
      });

      CHECK(F(0) == Approx(3));
      CHECK(F(4) == Approx(11));
      CHECK(F(5) == Approx(-1));
      CHECK(F(10) == Approx(-1));
    }
  }

  SECTION("2D")
  {
    Field<double, 2> F(11, 6);
    F.setCoordinateSystem(Uniform(0, 10), Uniform(10, 15));

    CHECK(F.getCoord(0, 0)[0] == Approx(0));
    CHECK(F.getCoord(0, 0)[1] == Approx(10));
    CHECK(F.getCoord(0, 5)[0] == Approx(0));
    CHECK(F.getCoord(0, 5)[1] == Approx(15));
    CHECK(F.getCoord(10, 0)[0] == Approx(10));
    CHECK(F.getCoord(10, 0)[1] == Approx(10));
    CHECK(F.getCoord(10, 5)[0] == Approx(10));
    CHECK(F.getCoord(10, 5)[1] == Approx(15));

    SECTION("1 arg signature")
    {
      F.set_f([](auto x) { return 2 * x[0] + 3 * x[1] + 4; });

      CHECK(F(0, 0) == Approx(34));
      CHECK(F(10, 0) == Approx(54));
      CHECK(F(0, 5) == Approx(49));
      CHECK(F(10, 5) == Approx(69));
    }

    SECTION("1 arg signature with optional return")
    {
      F.set(-1);
      F.set_f([](auto x) -> boost::optional<double> {
        if (x[0] < 5.5 && x[1] < 12) return 2 * x[0] + 3 * x[1] + 4;
        return boost::none;
      });

      CHECK(F(0, 0) == Approx(34));
      CHECK(F(10, 0) == Approx(-1));
      CHECK(F(0, 5) == Approx(-1));
      CHECK(F(10, 5) == Approx(-1));
    }
  }
}
