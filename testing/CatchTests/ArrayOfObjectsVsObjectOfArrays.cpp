#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"

#include "Field.hpp"
#include "Utils.h"
#include "fakeit.hpp"

struct Node {
  double T;
  double rho;
  double c;
  double kappa;
};

struct FieldCollection {
  Field<double, 1> T;
  Field<double, 1> rho;
  Field<double, 1> c;
  Field<double, 1> kappa;

  FieldCollection(int Nx)
      : T(Nx),
        rho(T.getCoordinateSystemPtr()),
        c(T.getCoordinateSystemPtr()),
        kappa(T.getCoordinateSystemPtr())
  {
  }

  CoordinateSystem<double, 1>& getCoordinateSystem()
  {
    return T.getCoordinateSystem();
  }
};

TEST_CASE("Array of Objects vs. Object of Arrays")
{
  int    Nt = 1000;
  int    Nx = 1000;
  double dt = 0.1;
  double dx = 0.1;

  SECTION("Array of Objects")
  {
    Field<Node, 1, double> Nodes(Nx);
    Nodes.getCoordinateSystem().set(Uniform(0., 10.));

    for (int i = 0; i < Nx; ++i) {
      Nodes(i).rho   = 2;
      Nodes(i).c     = 3;
      Nodes(i).kappa = 4;
      Nodes(i).T     = i * (Nx - 1 - i);
    }

    BENCHMARK("Conduction")
    {
      for (int n = 0; n < Nt; ++n) {
        for (int i = 1; i < Nx - 1; ++i) {
          Nodes(i).T = Nodes(i).kappa / (Nodes(i).rho * Nodes(i).c) *
                           (dt / dx) *
                           (Nodes(i - 1).T - 2 * Nodes(i).T + Nodes(i + 1).T) +
                       Nodes(i).T;
        }
      }
    };
  }

  SECTION("Object of Arrays")
  {
    FieldCollection Fields(Nx);
    Fields.T.getCoordinateSystem().set(Uniform(0., 10.));

    for (int i = 0; i < Nx; i++) {
      CHECK(Fields.rho.getCoord(i) == Approx(i * (10. / (Nx - 1))));
      CHECK(Fields.c.getCoord(i) == Approx(i * (10. / (Nx - 1))));
      CHECK(Fields.kappa.getCoord(i) == Approx(i * (10. / (Nx - 1))));
      CHECK(Fields.T.getCoord(i) == Approx(i * (10. / (Nx - 1))));
    }

    for (int i = 0; i < Nx; ++i) {
      Fields.rho(i)   = 2;
      Fields.c(i)     = 3;
      Fields.kappa(i) = 4;
      Fields.T(i)     = i * (Nx - 1 - i);
    }

    BENCHMARK("Conduction")
    {
      for (int n = 0; n < Nt; ++n) {
        for (int i = 1; i < Nx - 1; ++i) {
          Fields.T(i) =
              Fields.kappa(i) / (Fields.rho(i) * Fields.c(i)) * (dt / dx) *
                  (Fields.T(i - 1) - 2 * Fields.T(i) + Fields.T(i + 1)) +
              Fields.T(i);
        }
      }
    };
  }
}
