#include "catch.hpp"

#include <libField/CoordinateSystem.hpp>
#include "Utils.h"
#include "fakeit.hpp"

using namespace boost;

TEST_CASE("CoordinateSystem Usage")
{
  int    Nx = 11, Ny = 6, Nz = 21;
  double xmin = 0, xmax = 10, dx = (xmax - xmin) / (Nx - 1);
  double ymin = 0, ymax = 10, dy = (ymax - ymin) / (Ny - 1);
  double zmin = 0, zmax = 10, dz = (zmax - zmin) / (Nz - 1);

  CoordinateSystem<double, 3> Coordinates(Nx, Ny, Nz);

  CHECK(Coordinates.size(0) == Nx);
  CHECK(Coordinates.size(1) == Ny);
  CHECK(Coordinates.size(2) == Nz);
  CHECK(Coordinates.size(3) == 0);
  CHECK(Coordinates.size(-1) == Nx * Ny * Nz);

  Coordinates.set(Uniform(xmin, xmax), Uniform(ymin, ymax),
                  Uniform(zmin, zmax));

  for (int i = 0; i < Nx; i++)
    CHECK(Coordinates[0][i] == Approx(xmin + i * dx));
  for (int i = 0; i < Ny; i++)
    CHECK(Coordinates[1][i] == Approx(ymin + i * dy));
  for (int i = 0; i < Nz; i++)
    CHECK(Coordinates[2][i] == Approx(zmin + i * dz));

  for (int i = 0; i < Nx; i++)
    for (int j = 0; j < Ny; j++)
      for (int k = 0; k < Nz; k++) {
        auto X = Coordinates.getCoord(i, j, k);
        CHECK(X[0] == Approx(xmin + i * dx));
        CHECK(X[1] == Approx(ymin + j * dy));
        CHECK(X[2] == Approx(zmin + k * dz));
      }

  CHECK(Coordinates[0][0] != -1);
  Coordinates[0][0] = -1;
  CHECK(Coordinates[0][0] == -1);

  Coordinates.getAxis(0)[0] = -1;
  CHECK(Coordinates[0][0] == -1);

  Coordinates.getAxisPtr(0)->operator[](0) = -1;
  CHECK(Coordinates[0][0] == -1);

  auto ind = Coordinates.lower_bound(0.1, 0.1, 0.1);
  CHECK(ind[0] == 0);
  CHECK(ind[1] == 0);
  CHECK(ind[2] == 0);

  ind = Coordinates.lower_bound(1.1, 1.1, 1.1);
  CHECK(ind[0] == 1);
  CHECK(ind[1] == 0);
  CHECK(ind[2] == 2);

  ind = Coordinates.upper_bound(0.1, 0.1, 0.1);
  CHECK(ind[0] == 1);
  CHECK(ind[1] == 1);
  CHECK(ind[2] == 1);

  ind = Coordinates.upper_bound(1.1, 1.1, 1.1);
  CHECK(ind[0] == 2);
  CHECK(ind[1] == 1);
  CHECK(ind[2] == 3);

  // we can also pass in shared pointers for the axes.
  typedef CoordinateSystem<double, 3>::axis_type axis_type;
  int                                            NNx = 20;
  int                                            NNy = 5;
  int                                            NNz = Ny;
  std::shared_ptr<axis_type>                     x(new axis_type(extents[NNx]));
  for (int i = 0; i < NNx; i++) {
    x->operator[](i) = 2 * i;
  }
  CoordinateSystem<double, 3> Coordinates2(x, NNy, Coordinates.getAxisPtr(1));

  CHECK(Coordinates2.size(0) == NNx);
  CHECK(Coordinates2.size(1) == NNy);
  CHECK(Coordinates2.size(2) == NNz);
  CHECK(Coordinates2.size(3) == 0);
  CHECK(Coordinates2.size(-1) == NNx * NNy * NNz);

  // y axis has not been set yet, but the x and z axis are
  for (int i = 0; i < NNx; i++) CHECK(Coordinates2[0][i] == Approx(2 * i));
  for (int i = 0; i < NNz; i++)
    CHECK(Coordinates2[2][i] ==
          Approx(ymin + i * dy));  // z axis is a reference to the first y axis

  // set the y axis, but leave the others along
  Coordinates2.set(none, Uniform<double>(-5, 5), none);

  for (int i = 0; i < NNx; i++) CHECK(Coordinates2[0][i] == Approx(2 * i));
  for (int i = 0; i < NNy; i++)
    CHECK(Coordinates2[1][i] == Approx(-5 + i * 10. / (NNy - 1)));
  for (int i = 0; i < NNy; i++)
    CHECK(Coordinates2[2][i] == Approx(ymin + i * dy));

  auto ind2 = Coordinates2.lower_bound(0.1, 0.1, 0.1);
  CHECK(ind2[0] == 0);
  CHECK(ind2[1] == 2);
  CHECK(ind2[2] == 0);

  ind2 = Coordinates2.lower_bound(1.1, 1.1, 1.1);
  CHECK(ind2[0] == 0);
  CHECK(ind2[1] == 2);
  CHECK(ind2[2] == 0);

  ind2 = Coordinates2.upper_bound(0.1, 0.1, 0.1);
  CHECK(ind2[0] == 1);
  CHECK(ind2[1] == 3);
  CHECK(ind2[2] == 1);

  ind2 = Coordinates2.upper_bound(1.1, 1.1, 1.1);
  CHECK(ind2[0] == 1);
  CHECK(ind2[1] == 3);
  CHECK(ind2[2] == 1);
}

TEST_CASE("CoordinateSystem Range Setting")
{
  CoordinateSystem<double, 3> Coordinates(10, 10, 10);

  Coordinates.set(Uniform(1, 10), Geometric(0., 1., 2), Geometric(1., 0.1, 1.1));

  for (int i = 0; i < 10; i++)
    CHECK(Coordinates.getAxis(0)[i] == Approx(1 + i));

  double x  = 0;
  double dx = 1;
  double s  = 2;
  for (int i = 0; i < 10; i++) {
    CHECK(Coordinates.getAxis(1)[i] == Approx(x));
    x += dx;
    dx *= s;
  }
}

TEST_CASE("CoordinateSystem Slicing")
{
  int    Nx = 11, Ny = 6, Nz = 21;
  double xmin = 0, xmax = 10, dx = (xmax - xmin) / (Nx - 1);
  double ymin = 0, ymax = 10, dy = (ymax - ymin) / (Ny - 1);
  double zmin = 0, zmax = 10, dz = (zmax - zmin) / (Nz - 1);

  CoordinateSystem<double, 3> Coordinates(Nx, Ny, Nz);

  Coordinates.set(Uniform(xmin, xmax), Uniform(ymin, ymax),
                  Uniform(zmin, zmax));

  REQUIRE(Coordinates[0].size() == Nx);
  for (int i = 0; i < Nx; i++)
    CHECK(Coordinates[0][i] == Approx(xmin + i * dx));

  REQUIRE(Coordinates[1].size() == Ny);
  for (int i = 0; i < Ny; i++)
    CHECK(Coordinates[1][i] == Approx(ymin + i * dy));

  REQUIRE(Coordinates[2].size() == Nz);
  for (int i = 0; i < Nz; i++)
    CHECK(Coordinates[2][i] == Approx(zmin + i * dz));

  auto ind = Coordinates.lower_bound(2, 3, 4);
  CHECK(ind[0] == 2);
  CHECK(ind[1] == 1);
  CHECK(ind[2] == 8);

  auto Coordinates2 =
      Coordinates.slice(indices[index_range()][2][index_range()]);

  REQUIRE(Coordinates2[0].size() == Nx);
  for (int i = 0; i < Nx; i++)
    CHECK(Coordinates2[0][i] == Approx(xmin + i * dx));
  REQUIRE(Coordinates2[1].size() == Nz);
  for (int i = 0; i < Nz; i++)
    CHECK(Coordinates2[1][i] == Approx(zmin + i * dz));

  auto ind2 = Coordinates2.lower_bound(2, 4);
  CHECK(ind2[0] == 2);
  CHECK(ind2[1] == 8);

  auto Coordinates3 =
      Coordinates.slice(indices[index_range(2, Nx)][1][index_range(1, 10, 2)]);

  auto ind3 = Coordinates3.lower_bound(2, 4);
  CHECK(ind3[0] == 0);
  CHECK(ind3[1] == 3);

  REQUIRE(Coordinates3[0].size() == Nx - 2);
  for (int i = 2; i < Nx; i++)
    CHECK(Coordinates3[0][i - 2] == Approx(xmin + i * dx));

  REQUIRE(Coordinates3[1].size() == 5);
  CHECK(Coordinates3[1][0] == zmin + dz);
  CHECK(Coordinates3[1][1] == zmin + 3 * dz);
  CHECK(Coordinates3[1][2] == zmin + 5 * dz);
  CHECK(Coordinates3[1][3] == zmin + 7 * dz);
  CHECK(Coordinates3[1][4] == zmin + 9 * dz);

  Coordinates3.set(none, [](int i, int N) { return 0; });
  CHECK(Coordinates3[1][0] == 0);
  CHECK(Coordinates3[1][1] == 0);
  CHECK(Coordinates3[1][2] == 0);
  CHECK(Coordinates3[1][3] == 0);
  CHECK(Coordinates3[1][4] == 0);

  CHECK(Coordinates[2][0] == zmin);
  CHECK(Coordinates[2][1] == 0);
  CHECK(Coordinates[2][2] == zmin + 2 * dz);
  CHECK(Coordinates[2][3] == 0);
  CHECK(Coordinates[2][4] == zmin + 4 * dz);
}

TEST_CASE("CoordinateSystem Index Searching")
{
  CoordinateSystem<double, 3> Coordinates(11, 6, 21);
  Coordinates.set(Uniform(0., 10.), Uniform(0., 10.), Uniform(0., 10.));

  // std::cout << "===================================" << std::endl;

  typedef std::array<int, 3> A;
  A                          ind;

  ind = Coordinates.lower_bound(-1, -1, -1);
  CHECK_THAT(ind, IsEqualToArray<A>({-1, -1, -1}));
  ind = Coordinates.upper_bound(-1, -1, -1);
  CHECK_THAT(ind, IsEqualToArray<A>({0, 0, 0}));
  ind = Coordinates.nearest(-1, -1, -1);
  CHECK_THAT(ind, IsEqualToArray<A>({0, 0, 0}));

  ind = Coordinates.lower_bound(0.1, 0.1, 0.1);
  CHECK_THAT(ind, IsEqualToArray<A>({0, 0, 0}));
  ind = Coordinates.upper_bound(0.1, 0.1, 0.1);
  CHECK_THAT(ind, IsEqualToArray<A>({1, 1, 1}));
  ind = Coordinates.nearest(0.1, 0.1, 0.1);
  CHECK_THAT(ind, IsEqualToArray<A>({0, 0, 0}));

  ind = Coordinates.lower_bound(0.2, 0.2, 0.2);
  CHECK_THAT(ind, IsEqualToArray<A>({0, 0, 0}));
  ind = Coordinates.upper_bound(0.2, 0.2, 0.2);
  CHECK_THAT(ind, IsEqualToArray<A>({1, 1, 1}));
  ind = Coordinates.nearest(0.2, 0.2, 0.2);
  CHECK_THAT(ind, IsEqualToArray<A>({0, 0, 0}));

  ind = Coordinates.lower_bound(0.3, 0.3, 0.3);
  CHECK_THAT(ind, IsEqualToArray<A>({0, 0, 0}));
  ind = Coordinates.upper_bound(0.3, 0.3, 0.3);
  CHECK_THAT(ind, IsEqualToArray<A>({1, 1, 1}));
  ind = Coordinates.nearest(0.3, 0.3, 0.3);
  CHECK_THAT(ind, IsEqualToArray<A>({0, 0, 1}));

  ind = Coordinates.lower_bound(0.4, 0.4, 0.4);
  CHECK_THAT(ind, IsEqualToArray<A>({0, 0, 0}));
  ind = Coordinates.upper_bound(0.4, 0.4, 0.4);
  CHECK_THAT(ind, IsEqualToArray<A>({1, 1, 1}));
  ind = Coordinates.nearest(0.4, 0.4, 0.4);
  CHECK_THAT(ind, IsEqualToArray<A>({0, 0, 1}));

  ind = Coordinates.lower_bound(0.5, 0.5, 0.5);
  CHECK_THAT(ind, IsEqualToArray<A>({0, 0, 1}));
  ind = Coordinates.upper_bound(0.5, 0.5, 0.5);
  CHECK_THAT(ind, IsEqualToArray<A>({1, 1, 2}));
  ind = Coordinates.nearest(0.5, 0.5, 0.5);
  CHECK_THAT(ind, IsEqualToArray<A>({1, 0, 1}));

  ind = Coordinates.lower_bound(0.6, 0.6, 0.6);
  CHECK_THAT(ind, IsEqualToArray<A>({0, 0, 1}));
  ind = Coordinates.upper_bound(0.6, 0.6, 0.6);
  CHECK_THAT(ind, IsEqualToArray<A>({1, 1, 2}));
  ind = Coordinates.nearest(0.6, 0.6, 0.6);
  CHECK_THAT(ind, IsEqualToArray<A>({1, 0, 1}));

  ind = Coordinates.lower_bound(0.7, 0.7, 0.7);
  CHECK_THAT(ind, IsEqualToArray<A>({0, 0, 1}));
  ind = Coordinates.upper_bound(0.7, 0.7, 0.7);
  CHECK_THAT(ind, IsEqualToArray<A>({1, 1, 2}));
  ind = Coordinates.nearest(0.7, 0.7, 0.7);
  CHECK_THAT(ind, IsEqualToArray<A>({1, 0, 1}));

  ind = Coordinates.lower_bound(0.8, 0.8, 0.8);
  CHECK_THAT(ind, IsEqualToArray<A>({0, 0, 1}));
  ind = Coordinates.upper_bound(0.8, 0.8, 0.8);
  CHECK_THAT(ind, IsEqualToArray<A>({1, 1, 2}));
  ind = Coordinates.nearest(0.8, 0.8, 0.8);
  CHECK_THAT(ind, IsEqualToArray<A>({1, 0, 2}));

  ind = Coordinates.lower_bound(0.9, 0.9, 0.9);
  CHECK_THAT(ind, IsEqualToArray<A>({0, 0, 1}));
  ind = Coordinates.upper_bound(0.9, 0.9, 0.9);
  CHECK_THAT(ind, IsEqualToArray<A>({1, 1, 2}));
  ind = Coordinates.nearest(0.9, 0.9, 0.9);
  CHECK_THAT(ind, IsEqualToArray<A>({1, 0, 2}));

  ind = Coordinates.lower_bound(1.0, 1.0, 1.0);
  CHECK_THAT(ind, IsEqualToArray<A>({1, 0, 2}));
  ind = Coordinates.upper_bound(1.0, 1.0, 1.0);
  CHECK_THAT(ind, IsEqualToArray<A>({2, 1, 3}));
  ind = Coordinates.nearest(1.0, 1.0, 1.0);
  CHECK_THAT(ind, IsEqualToArray<A>({1, 1, 2}));

  ind = Coordinates.lower_bound(1.01, 1.01, 1.01);
  CHECK_THAT(ind, IsEqualToArray<A>({1, 0, 2}));
  ind = Coordinates.upper_bound(1.01, 1.01, 1.01);
  CHECK_THAT(ind, IsEqualToArray<A>({2, 1, 3}));

  ind = Coordinates.lower_bound(2.01, 2.01, 2.01);
  CHECK_THAT(ind, IsEqualToArray<A>({2, 1, 4}));
  ind = Coordinates.upper_bound(2.01, 2.01, 2.01);
  CHECK_THAT(ind, IsEqualToArray<A>({3, 2, 5}));

  ind = Coordinates.lower_bound(9.01, 9.01, 9.01);
  CHECK_THAT(ind, IsEqualToArray<A>({9, 4, 18}));
  ind = Coordinates.upper_bound(9.01, 9.01, 9.01);
  CHECK_THAT(ind, IsEqualToArray<A>({10, 5, 19}));

  ind = Coordinates.lower_bound(10.01, 10.01, 10.01);
  CHECK_THAT(ind, IsEqualToArray<A>({10, 5, 20}));
  ind = Coordinates.upper_bound(10.01, 10.01, 10.01);
  CHECK_THAT(ind, IsEqualToArray<A>({11, 6, 21}));

  ind = Coordinates.lower_bound(11, 11, 11);
  CHECK_THAT(ind, IsEqualToArray<A>({10, 5, 20}));
  ind = Coordinates.upper_bound(11, 11, 11);
  CHECK_THAT(ind, IsEqualToArray<A>({11, 6, 21}));
  ind = Coordinates.nearest(11, 11, 11);
  CHECK_THAT(ind, IsEqualToArray<A>({10, 5, 20}));
}

TEST_CASE("getCoord Interface")
{
  SECTION("1D Interface")
  {
    CoordinateSystem<double, 1> Coordinates(11);
    Coordinates.set(Uniform(5, 6));

    CHECK(Coordinates[0][0] == Approx(5));
    CHECK(Coordinates[0][1] == Approx(5.1));

    {
      auto x = Coordinates.getCoord(5);
      CHECK(x == Approx(5.5));
    }

    {
      std::array<int, 1> ind{5};
      auto               x = Coordinates.getCoord(ind[0]);
      CHECK(x == Approx(5.5));
    }

    {
      std::array<int, 1> ind{5};
      auto               x = Coordinates.getCoord(ind);
      CHECK(x[0] == Approx(5.5));
    }
  }
  SECTION("2D Interface") {}
}
