#include <H5Cpp.h>
#include <exception>
#include <string>
#include "./Field.hpp"

namespace detail {
template <typename T>
const H5::PredType& get_hdf5_dtype_for_type()
{
  throw std::runtime_error(
      "This function should never be called. If you are seeing this, then you "
      "called the function with an unsupported data type. Please provide a "
      "template specialization for your data type that returns the correct "
      "hdf5 datatype.");
};

template <>
const H5::PredType& get_hdf5_dtype_for_type<long double>()
{
  return H5::PredType::NATIVE_LDOUBLE;
};
template <>
const H5::PredType& get_hdf5_dtype_for_type<double>()
{
  return H5::PredType::NATIVE_DOUBLE;
};
template <>
const H5::PredType& get_hdf5_dtype_for_type<float>()
{
  return H5::PredType::NATIVE_FLOAT;
};

}

/**
 * Writes a field to a hdf5 container, which may be a file or group.
 *
 * Each coordinate axis and the field data are written
 * to separate datasets. Axes are written to datasets named
 * 'axis {i}', where '{i}' is the axis number (zero offset).
 * The field data is written to a dataset named 'field'.
 *
 * For example, a 2D, 5x10 field will be written to three datasets.
 *
 * "axis 0" will contain the 5 coordinates of the first dimension.
 * "axis 1" will contain the 10 coordinates of the second dimension.
 * "field" will contain the 50 elements of the field.
 *
 */
template <typename ST, typename FT, size_t N, typename CT>
auto hdf5write(ST& container, const Field<FT, N, CT>& f) -> decltype(container.createGroup(std::string()),void())
{

  hsize_t dims[N];
  for (int i = 0; i < N; ++i) {
    dims[i] = f.size(i);
  }

  for (int i = 0; i < N; ++i) {
    H5::DataSpace dspace(1, &dims[i]);
    auto dset = container.createDataSet(("axis " + std::to_string(i)).c_str(),
                                   detail::get_hdf5_dtype_for_type<CT>(), dspace);
    dset.write(f.getAxis(i).data(), detail::get_hdf5_dtype_for_type<CT>());
    dset.close();
  }

  H5::DataSpace dspace(N, dims);

  auto dset =
      container.createDataSet("field", detail::get_hdf5_dtype_for_type<FT>(), dspace);
  dset.write(f.data(), detail::get_hdf5_dtype_for_type<FT>());
  dset.close();

}

/**
 * Writes a field to a file using the HDF5 format.
 *
 * Each coordinate axis and the field data are written
 * to separate datasets. Axes are written to datasets named
 * 'axis {i}', where '{i}' is the axis number (zero offset).
 * The field data is written to a dataset named 'field'.
 *
 * For example, a 2D, 5x10 field will be written to three datasets.
 *
 * "axis 0" will contain the 5 coordinates of the first dimension.
 * "axis 1" will contain the 10 coordinates of the second dimension.
 * "field" will contain the 50 elements of the field.
 *
 */
template <typename FT, size_t N, typename CT>
void hdf5write(std::string name, const Field<FT, N, CT>& f, decltype(H5F_ACC_TRUNC) acc = H5F_ACC_TRUNC)
{
  H5::H5File file(name.c_str(), acc);
  hdf5write( file, f);
  file.close();
}

template <typename ST, typename FT, size_t N, typename CT>
auto hdf5read(ST& container, Field<FT, N, CT>& f) -> decltype(container.createGroup(std::string()),void())
{
  hsize_t ddims[N];
  auto dset = container.openDataSet("field");
  auto dspace = dset.getSpace();
  if (dspace.getSimpleExtentNdims() != N)
    throw std::runtime_error("Cannot read field from container. Dimensions of field stored in container (" +
                             std::to_string(dspace.getSimpleExtentNdims()) +
                             ") do not match the field being read into (" +
                             std::to_string(N) + ").");
  dspace.getSimpleExtentDims(ddims);

  std::array<size_t, N> dims;
  for (int i = 0; i < N; ++i) dims[i] = ddims[i];

  f = Field<FT, N, CT>(dims);
  dset.read(f.data(), detail::get_hdf5_dtype_for_type<FT>());

  for (int i = 0; i < N; ++i) {
    auto dset = container.openDataSet(("axis " + std::to_string(i)).c_str());
    dset.read(f.getAxis(i).data(), detail::get_hdf5_dtype_for_type<CT>());
  }
}

/**
 * Reads a field from an HdF5 file.
 *
 * This function assumes that the file is structured in the way written by hdf5write.
 */
template <typename FT, size_t N, typename CT>
void hdf5read(std::string name, Field<FT, N, CT>& f)
{
  H5::H5File file(name.c_str(), H5F_ACC_RDONLY);
  try {
  hdf5read(file, f);
  } catch( std::runtime_error& e)
  {
    throw std::runtime_error("There was an error reading field from '" + name +". "+e.what());
  }

  file.close();
}

