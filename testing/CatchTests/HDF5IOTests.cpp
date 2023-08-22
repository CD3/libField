#define CATCH_CONFIG_ENABLE_BENCHMARKING
#ifdef HAVE_HDF5_CPP
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <libField/Field.hpp>
#include <libField/HDF5.hpp>

#include "Utils.h"

TEST_CASE("HDF5 Read and Write")
{
  SECTION("1D Fields")
  {
    SECTION("double out float in")
    {
      Field<double, 1> F(10);
      F.setCoordinateSystem(Uniform(0, 2));
      F.set_f([](auto x) { return x[0] * x[0]; });

      hdf5write("1D-Field.h5", F);

      Field<float, 1> G;
      hdf5read("1D-Field.h5", G);

      CHECK(G.size() == 10);
      CHECK(G.getCoord(0) == Catch::Approx(0));
      CHECK(G.getCoord(9) == Catch::Approx(2));
      CHECK(G(0) == Catch::Approx(0));
      CHECK(G(9) == Catch::Approx(4));
    }

    SECTION("double/float out float/double in")
    {
      Field<double, 1, float> F(10);
      F.setCoordinateSystem(Uniform(0, 2));
      F.set_f([](auto x) { return x[0] * x[0]; });

      hdf5write("1D-Field.h5", F);

      Field<float, 1, double> G;
      hdf5read("1D-Field.h5", G);

      CHECK(G.size() == 10);
      CHECK(G.getCoord(0) == Catch::Approx(0));
      CHECK(G.getCoord(9) == Catch::Approx(2));
      CHECK(G(0) == Catch::Approx(0));
      CHECK(G(9) == Catch::Approx(4));
    }

    SECTION("writing to groups")
    {
      Field<double, 1> F(10);
      F.setCoordinateSystem(Uniform(0, 2));
      F.set_f([](auto x) { return x[0] * x[0]; });

      // hdf5write( "1D-Field.h5", "/electric field", F );
      // hdf5write( "1D-Field.h5", "/temperature", F );
    }
    SECTION("Mismatched dimensions throws")
    {
      Field<double, 1> F(10);
      F.setCoordinateSystem(Uniform(0, 2));
      F.set(0.0);

      hdf5write("1D-Field.h5", F);

      Field<float, 2> G;
      CHECK_THROWS(hdf5read("1D-Field.h5", G));
    }
  }

  SECTION("2D Fields")
  {
    SECTION("double out float in")
    {
      Field<double, 2> F(10, 20);
      F.setCoordinateSystem(Uniform(0, 2), Uniform(0, 4));
      F.set_f([](auto x) { return x[0] * x[0] + x[1] * x[1]; });

      hdf5write("2D-Field.h5", F);

      Field<float, 2> G;
      hdf5read("2D-Field.h5", G);

      CHECK(G.size() == 200);
      CHECK(G.size(0) == 10);
      CHECK(G.size(1) == 20);
      CHECK(G.getCoord(0, 0)[0] == Catch::Approx(0));
      CHECK(G.getCoord(9, 0)[0] == Catch::Approx(2));
      CHECK(G.getCoord(0, 19)[1] == Catch::Approx(4));
      CHECK(G(0, 0) == Catch::Approx(0));
      CHECK(G(9, 0) == Catch::Approx(4));
      CHECK(G(0, 19) == Catch::Approx(16));
      CHECK(G(9, 19) == Catch::Approx(20));
    }

    SECTION("float out double in")
    {
      Field<float, 2> F(10, 20);
      F.setCoordinateSystem(Uniform(0, 2), Uniform(0, 4));
      F.set_f([](auto x) { return x[0] * x[0] + x[1] * x[1]; });

      hdf5write("2D-Field.h5", F);

      Field<double, 2> G;
      hdf5read("2D-Field.h5", G);

      CHECK(G.size() == 200);
      CHECK(G.size(0) == 10);
      CHECK(G.size(1) == 20);
      CHECK(G.getCoord(0, 0)[0] == Catch::Approx(0));
      CHECK(G.getCoord(9, 0)[0] == Catch::Approx(2));
      CHECK(G.getCoord(0, 19)[1] == Catch::Approx(4));
      CHECK(G(0, 0) == Catch::Approx(0));
      CHECK(G(9, 0) == Catch::Approx(4));
      CHECK(G(0, 19) == Catch::Approx(16));
      CHECK(G(9, 19) == Catch::Approx(20));
    }
  }

  SECTION("Reading from Nested Datasets")
  {
    Field<double, 1> F(10);
    F.setCoordinateSystem(Uniform(0, 2));
    F.set_f([](auto x) { return x[0] * x[0]; });

    H5::H5File file("ManyFields.h5", H5F_ACC_TRUNC);
    auto       g1 = file.createGroup("Field 1");
    hdf5write(g1, F);
    g1.close();

    auto g2 = file.createGroup("Experiment 1").createGroup("Data 1").createGroup("Trial 1");
    hdf5write(g2, F);
    g2.close();

    file.close();

    {
      Field<double, 1> G;
      H5::H5File file("ManyFields.h5", H5F_ACC_RDONLY);
      std::vector<std::string> path;
      path.push_back("Field 1");
      hdf5read( file, path, G);

      CHECK(G.size() == 10);
      CHECK(G(0) == 0);
      CHECK(G(9) == 4);
      file.close();
    }

    {
      Field<double, 1> G;
      H5::H5File file("ManyFields.h5", H5F_ACC_RDONLY);
      std::vector<std::string> path(3);
      path[0] = "Experiment 1";
      path[1] = "Data 1";
      path[2] = "Trial 1";
      hdf5read( file, path, G);

      CHECK(G.size() == 10);
      CHECK(G(0) == 0);
      CHECK(G(9) == 4);
      file.close();
    }

    {
      Field<double, 1> G;
      H5::H5File file("ManyFields.h5", H5F_ACC_RDONLY);
      hdf5read( file, "/Experiment 1/Data 1/Trial 1", G);

      CHECK(G.size() == 10);
      CHECK(G(0) == 0);
      CHECK(G(9) == 4);
      file.close();
    }

    {
      Field<double, 1> G;
      hdf5read( "ManyFields.h5", "Experiment 1/Data 1/Trial 1/", G);

      CHECK(G.size() == 10);
      CHECK(G(0) == 0);
      CHECK(G(9) == 4);
      file.close();
    }
  }

  SECTION("Reading other formats")
  {
    {
      // Write a file with several different datasets in it
      H5::H5File file("ManyDatasets.h5", H5F_ACC_TRUNC);

      {
        double data[3][5];
        for (int i = 0; i < 3; ++i)
          for (int j = 0; j < 5; ++j) data[i][j] = i + j;
        hsize_t dims[2];
        dims[0] = 3;
        dims[1] = 5;
        H5::DataSpace dspace(2, dims);
        auto          dset = file.createDataSet("Small 2D Array",
                                       H5::PredType::NATIVE_DOUBLE, dspace);
        dset.write(data, H5::PredType::NATIVE_DOUBLE);
        dset.close();
      }

      {
        double data[10];
        for (int i = 0; i < 10; ++i) data[i] = i;
        hsize_t dims[1];
        dims[0] = 10;
        H5::DataSpace dspace(1, dims);
        auto          dset = file.createDataSet("Small 1D Array",
                                       H5::PredType::NATIVE_DOUBLE, dspace);
        dset.write(data, H5::PredType::NATIVE_DOUBLE);
        dset.close();
      }

      {
        double data[2][3][4];
        for (int i = 0; i < 2; ++i)
          for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 4; ++k) data[i][j][k] = i + j + k;
        hsize_t dims[3];
        dims[0] = 2;
        dims[1] = 3;
        dims[2] = 4;

        H5::DataSpace dspace(3, dims);
        auto          dset = file.createDataSet("Small 3D Array",
                                       H5::PredType::NATIVE_DOUBLE, dspace);
        dset.write(data, H5::PredType::NATIVE_DOUBLE);
        dset.close();
      }

      {
        double  data = 0.1;
        hsize_t dims[1];
        dims[0] = 1;
        H5::DataSpace dspace(1, dims);
        auto          dset =
            file.createDataSet("dx", H5::PredType::NATIVE_DOUBLE, dspace);
        dset.write(&data, H5::PredType::NATIVE_DOUBLE);
        dset.close();
      }
      {
        double  data = 0.2;
        hsize_t dims[1];
        dims[0] = 1;
        H5::DataSpace dspace(1, dims);
        auto          dset =
            file.createDataSet("dy", H5::PredType::NATIVE_DOUBLE, dspace);
        dset.write(&data, H5::PredType::NATIVE_DOUBLE);
        dset.close();
      }
      {
        double  data = 0.3;
        hsize_t dims[1];
        dims[0] = 1;
        H5::DataSpace dspace(1, dims);
        auto          dset =
            file.createDataSet("dz", H5::PredType::NATIVE_DOUBLE, dspace);
        dset.write(&data, H5::PredType::NATIVE_DOUBLE);
        dset.close();
      }
      file.close();
    }

    // now read datasets
    H5::H5File file("ManyDatasets.h5", H5F_ACC_RDONLY);
    SECTION("1D Field")
    {
      Field<double, 1> f;
      auto             dset = file.openDataSet("Small 1D Array");
      hdf5read(dset, f);
      CHECK(f.size() == 10);
      CHECK(f(0) == Catch::Approx(0));
      CHECK(f(1) == Catch::Approx(1));
      CHECK(f(9) == Catch::Approx(9));

      CHECK(f.getAxis(0)[0] == Catch::Approx(0));
      CHECK(f.getAxis(0)[9] == Catch::Approx(9));
    }
    SECTION("2D Field")
    {
      Field<double, 2> f;
      auto             dset = file.openDataSet("Small 2D Array");
      hdf5read(dset, f);
      CHECK(f.size() == 15);
      CHECK(f.size(0) == 3);
      CHECK(f.size(1) == 5);
      CHECK(f(0, 0) == Catch::Approx(0));
      CHECK(f(1, 0) == Catch::Approx(1));
      CHECK(f(0, 1) == Catch::Approx(1));
      CHECK(f(1, 1) == Catch::Approx(2));
      CHECK(f(2, 4) == Catch::Approx(6));

      CHECK(f.getAxis(0)[0] == Catch::Approx(0));
      CHECK(f.getAxis(0)[2] == Catch::Approx(2));
      CHECK(f.getAxis(1)[0] == Catch::Approx(0));
      CHECK(f.getAxis(1)[4] == Catch::Approx(4));
    }

    SECTION("3D Field")
    {
      Field<float, 3> f;
      auto            dset = file.openDataSet("Small 3D Array");
      hdf5read(dset, f);
      CHECK(f.size() == 24);
      CHECK(f.size(0) == 2);
      CHECK(f.size(1) == 3);
      CHECK(f.size(2) == 4);
      CHECK(f(0, 0, 0) == Catch::Approx(0));
      CHECK(f(1, 0, 0) == Catch::Approx(1));
      CHECK(f(0, 1, 0) == Catch::Approx(1));
      CHECK(f(1, 1, 1) == Catch::Approx(3));
      CHECK(f(1, 2, 3) == Catch::Approx(6));

      CHECK(f.getAxis(0)[0] == Catch::Approx(0));
      CHECK(f.getAxis(0)[1] == Catch::Approx(1));
      CHECK(f.getAxis(1)[0] == Catch::Approx(0));
      CHECK(f.getAxis(1)[2] == Catch::Approx(2));
      CHECK(f.getAxis(2)[0] == Catch::Approx(0));
      CHECK(f.getAxis(2)[3] == Catch::Approx(3));
    }
    file.close();
  }

  SECTION("Read Errors")
  {
    H5::H5File file("ReadErrors.h5", H5F_ACC_TRUNC);

    {
      auto   group = file.createGroup("Mismatched Axis Size");
      double data[3][5];
      for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 5; ++j) data[i][j] = i + j;
      hsize_t dims[2];
      dims[0] = 3;
      dims[1] = 5;

      {
        H5::DataSpace dspace(2, dims);
        auto          dset =
            group.createDataSet("field", H5::PredType::NATIVE_DOUBLE, dspace);
        dset.write(data, H5::PredType::NATIVE_DOUBLE);
        dset.close();
      }

      {
        dims[0] = 2;
        H5::DataSpace dspace(1, &dims[0]);
        auto          dset =
            group.createDataSet("axis 0", H5::PredType::NATIVE_DOUBLE, dspace);
        dset.write(dims, H5::PredType::NATIVE_DOUBLE);
        dset.close();
      }

      {
        H5::DataSpace dspace(1, &dims[1]);
        auto          dset =
            group.createDataSet("axis 1", H5::PredType::NATIVE_DOUBLE, dspace);
        dset.write(&data[0], H5::PredType::NATIVE_DOUBLE);
        dset.close();
      }

      group.close();
    }

    file.close();

    SECTION("Axis size mismatch")
    {
      Field<float, 2> f;
      H5::H5File      ifile("ReadErrors.h5", H5F_ACC_RDONLY);
      auto            group = ifile.openGroup("Mismatched Axis Size");
      CHECK_THROWS_WITH(
          hdf5read(group, f),
          Catch::Equals("Cannot read axis data from 'axis 0'. Size (2) does "
                        "not match size expected from field data (3)."));
    }
  }

  SECTION("Writing multiple fields to the same file")
  {
    Field<double,1> T(10);
    T.setCoordinateSystem(Uniform(0,2));
    Field<float,1> U(20);
    U.setCoordinateSystem(Uniform(0,1));

    hdf5write( "MultipleFieldWrite.h5", "1D/Tvst", T, H5F_ACC_TRUNC );
    hdf5write( "MultipleFieldWrite.h5", "1D/Uvst", U, H5F_ACC_RDWR );

    Field<double,2> E(10,10);
    E.setCoordinateSystem(Uniform(0,2),Uniform(0,3));
    Field<float,2> F(20,10);
    F.setCoordinateSystem(Uniform(-1,1),Uniform(0,2));

    hdf5write( "MultipleFieldWrite.h5", "2D/Evst", E, H5F_ACC_RDWR );
    hdf5write( "MultipleFieldWrite.h5", "2D/Fvst", F, H5F_ACC_RDWR );

    Field<double,1> _1DF;
    Field<float,2> _2DF;

    hdf5read("MultipleFieldWrite.h5", "1D/Tvst", _1DF);
    CHECK(_1DF.size() == 10);
    CHECK(_1DF.getAxis(0)[0] == Catch::Approx(0));
    CHECK(_1DF.getAxis(0)[9] == Catch::Approx(2));

    hdf5read("MultipleFieldWrite.h5", "1D/Uvst", _1DF);
    CHECK(_1DF.size() == 20);
    CHECK(_1DF.getAxis(0)[0] == Catch::Approx(0));
    CHECK(_1DF.getAxis(0)[19] == Catch::Approx(1));

    hdf5read("MultipleFieldWrite.h5", "2D/Evst", _2DF);
    CHECK(_2DF.size() == 100);
    CHECK(_2DF.size(0) == 10);
    CHECK(_2DF.size(1) == 10);
    CHECK(_2DF.getAxis(0)[0] == Catch::Approx(0));
    CHECK(_2DF.getAxis(0)[9] == Catch::Approx(2));
    CHECK(_2DF.getAxis(1)[0] == Catch::Approx(0));
    CHECK(_2DF.getAxis(1)[9] == Catch::Approx(3));

    hdf5read("MultipleFieldWrite.h5", "2D/Fvst", _2DF);
    CHECK(_2DF.size() == 200);
    CHECK(_2DF.size(0) == 20);
    CHECK(_2DF.size(1) == 10);
    CHECK(_2DF.getAxis(0)[0] == Catch::Approx(-1));
    CHECK(_2DF.getAxis(0)[19] == Catch::Approx(1));
    CHECK(_2DF.getAxis(1)[0] == Catch::Approx(0));
    CHECK(_2DF.getAxis(1)[9] == Catch::Approx(2));

  }
}
#endif
