#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <vector>

#include <boost/multi_array.hpp>

#include "Utils.h"

using namespace boost;

template<typename T, long unsigned int N>
using ma = multi_array<T, N>;
template<typename T, long unsigned int N>
using I = typename ma<T, N>::index;
typedef multi_array_types::index_range R;

TEST_CASE("Boost.Multiarray Examples", "[example]")
{
  int           Nx = 2, Ny = 3, Nz = 4;
  ma<double, 3> cube(extents[2][3][4]);

  for(int i = 0; i < Nx; ++i)
    for(int j = 0; j < Ny; ++j)
      for(int k = 0; k < Nz; ++k) cube[i][j][k] = i * j * k;

  for(int i = 0; i < Nx; ++i)
    for(int j = 0; j < Ny; ++j)
      for(int k = 0; k < Nz; ++k) CHECK(cube[i][j][k] == Catch::Approx(i * j * k));

  auto plane = cube[indices[R()][1][R()]];

  for(int i = 0; i < Nx; ++i)
    for(int k = 0; k < Nz; ++k) CHECK(plane[i][k] == Catch::Approx(i * 1 * k));

  plane[0][0] = -10;

  CHECK(plane[0][0] == Catch::Approx(-10));
  CHECK(cube[0][0][0] == Catch::Approx(0));
  CHECK(cube[0][1][0] == Catch::Approx(-10));

  cube[0][1][0] = 0 * 1 * 0;

  auto line = plane[indices[1][R()]];

  CHECK(line[0] == plane[1][0]);
  CHECK(line[0] == cube[1][1][0]);

  line[2] = -10;

  CHECK(line[2] == -10);
  CHECK(plane[1][2] == -10);
  CHECK(cube[1][1][2] == -10);
}

TEST_CASE("Boost.Multiarray Complex Matrix", "[example]")
{
  int N = 3;

  ma<double, 3> A(extents[N][N][2]);

  A[0][0][0] = 1;
  A[0][0][1] = 2;
  A[0][1][0] = 1;
  A[0][1][1] = 2;
  A[0][2][0] = 1;
  A[0][2][1] = 2;
  A[1][0][0] = 1;
  A[1][0][1] = 2;
  A[1][1][0] = 1;
  A[1][1][1] = 2;
  A[1][2][0] = 1;
  A[1][2][1] = 2;
  A[2][0][0] = 1;
  A[2][0][1] = 2;
  A[2][1][0] = 1;
  A[2][1][1] = 2;
  A[2][2][0] = 1;
  A[2][2][1] = 2;

  auto Ar = A[indices[R()][R()][0]];
  auto Ai = A[indices[R()][R()][1]];

  for(int i = 0; i < N; ++i)
    for(int j = 0; j < N; ++j) CHECK(Ar[i][j] == 1);

  for(int i = 0; i < N; ++i)
    for(int j = 0; j < N; ++j) CHECK(Ai[i][j] == 2);

  int  i  = 0;
  auto AA = A[indices[R()][R()][i]];

  for(int i = 0; i < N; ++i)
    for(int j = 0; j < N; ++j) CHECK(AA[i][j] == 1);
}

TEST_CASE("Boost.Multiarray Dynamic Slicing", "[example]")
{
  int N = 3;

  ma<double, 2> A(extents[N][N]);
  for(int i = 0; i < N; i++)
    for(int j = 0; j < N; j++) A[i][j] = i * j;

  for(int i = 0; i < N; ++i)
    for(int j = 0; j < N; ++j) CHECK(A[i][j] == i * j);

  int  d  = 1;
  auto Av = A[indices[R()][d]];

  for(int i = 0; i < N; ++i) CHECK(Av[i] == i * 1);
}

TEST_CASE("Boost.Multiarray Index Manipulation", "[example]")
{
  int           N = 3;
  ma<double, 2> A(extents[N][N]);
  for(int i = 0; i < N; i++)
    for(int j = 0; j < N; j++) A[i][j] = i * j;

  CHECK(A.data() == A.origin());

  array<int, 2> off = {1, 2};
  A.reindex(off);
  // now A[1][2] refers to A[0][0]
  // now A[2][3] refers to A[1][1]
  // etc.
  CHECK(A[1][2] == 0);
  CHECK(A[1][2] == 0);
  CHECK(A[2][3] == 1);

  for(int i = 1; i < N + 1; i++) {
    for(int j = 2; j < N + 2; j++) {
      CHECK(A[i][j] == (i - 1) * (j - 2));
    }
  }

  // origin should now be shifted
  CHECK(A.data() != A.origin());

  off = {-1, -2};
  A.reindex(off);
  // now A[-1][-2] refers to A[0][0]
  // now A[-2][-3] refers to A[1][1]
  // etc.
  CHECK(A[-1][-2] == 0);
  CHECK(A[-1][-2] == 0);
  CHECK(A[0][-1] == 1);

  for(int i = -1; i < N - 1; i++) {
    for(int j = -2; j < N - 2; j++) {
      CHECK(A[i][j] == (i + 1) * (j + 2));
    }
  }

  // origin should still be shifted
  CHECK(A.data() != A.origin());

  off = {0, 0};
  A.reindex(off);

  // origin should be back data()
  CHECK(A.data() == A.origin());
}

TEST_CASE("Boost.Multiarray Storage Order", "")
{
  int           N = 3;
  ma<double, 2> A(extents[N][N], c_storage_order());
  ma<double, 2> B(extents[N][N], fortran_storage_order());

  for(int i = 0; i < N; i++) {
    for(int j = 0; j < N; j++) {
      A[i][j] = 10 * i + j;
      B[i][j] = 10 * i + j;
    }
  }

  // have a 3x3 matrix
  //
  //   0  1  2
  //
  //  10 11 12
  //
  //  20 21 22
  //
  //

  CHECK(A.data()[0] == 0);
  CHECK(A.data()[1] == 1);
  CHECK(A.data()[2] == 2);
  CHECK(A.data()[3] == 10);

  CHECK(B.data()[0] == 0);
  CHECK(B.data()[1] == 10);
  CHECK(B.data()[2] == 20);
  CHECK(B.data()[3] == 1);
}
