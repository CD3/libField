#include <H5Cpp.h>
#include <exception>
#include <string>
#include <iostream>
#include <boost/algorithm/string.hpp>
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
}

template <>
inline const H5::PredType& get_hdf5_dtype_for_type<long double>()
{
  return H5::PredType::NATIVE_LDOUBLE;
}
template <>
inline const H5::PredType& get_hdf5_dtype_for_type<double>()
{
  return H5::PredType::NATIVE_DOUBLE;
}
template <>
inline const H5::PredType& get_hdf5_dtype_for_type<float>()
{
  return H5::PredType::NATIVE_FLOAT;
}

template<typename CT>
bool path_exists( const CT& container, std::string group )
{
  return H5Lexists(container.getId(), group.c_str(), H5P_DEFAULT);
}

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
  for (size_t i = 0; i < N; ++i) {
    dims[i] = f.size(i);
  }

  for (size_t i = 0; i < N; ++i) {
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

template <typename FT, size_t N, typename CT>
void hdf5write(std::string name, std::vector<std::string> path, const Field<FT, N, CT>& f, decltype(H5F_ACC_TRUNC) acc = H5F_ACC_RDWR)
{
  H5::H5File file(name.c_str(), acc);
  H5::Group group = file.openGroup("/");

  for( auto &elem: path )
  {
    if( detail::path_exists(group,elem) )
      group = group.openGroup(elem);
    else
      group = group.createGroup(elem);
  }
  hdf5write( group, f);
  file.close();
}

/**
 * Writes a field to a specified group in an HDF5 files.
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
void hdf5write(std::string name, std::string path, const Field<FT, N, CT>& f, decltype(H5F_ACC_TRUNC) acc = H5F_ACC_TRUNC)
{
  auto is_slash = [](char c){return c=='/';};
  boost::trim_if(path, is_slash);
  std::vector<std::string> elems;
  boost::split(elems, path, is_slash);
  hdf5write(name,elems,f,acc);
}

/*
 * Reads an HDF5 dataset into a field. The dataset is assumed to be an N-D array
 * and is read directly into the field data. Coordinates are set to integer indices.
 *
 * This function is useful for reading data written by some other application into a field.
 */
template <typename FT, size_t N, typename CT>
void hdf5read(H5::DataSet& dset, Field<FT, N, CT>& f)
{
  auto dspace = dset.getSpace();
  if (dspace.getSimpleExtentNdims() != N)
    throw std::runtime_error("Cannot read field from dataset. Dimensions of field stored in dataset (" +
                             std::to_string(dspace.getSimpleExtentNdims()) +
                             ") do not match the field being read into (" +
                             std::to_string(N) + ").");
  hsize_t ddims[N];
  dspace.getSimpleExtentDims(ddims);

  std::array<size_t, N> dims;
  for (size_t i = 0; i < N; ++i) dims[i] = ddims[i];

  f = Field<FT, N, CT>(dims);

  dset.read(f.data(), detail::get_hdf5_dtype_for_type<FT>());

  // set all coordinates to indices
  for(size_t n = 0; n < N; ++n)
  {
    for(size_t i = 0; i < f.size(n); ++i)
    {
      f.getAxis(n)[i] = i;
    }
  }

}


/*
 * Reads a field from an HDF5 container (either a group, or a file).
 *
 * @param container  the HDF5 container (file or group) to read from.
 * @param f the field to read data into.
 */
template <typename ST, typename FT, size_t N, typename CT>
auto hdf5read(ST& container, Field<FT, N, CT>& f) -> decltype(container.createGroup(std::string()),void())
{
  auto dset = container.openDataSet("field");
  auto dspace = dset.getSpace();
  if (dspace.getSimpleExtentNdims() != N)
    throw std::runtime_error("Cannot read field from container. Dimensions of field stored in container (" +
                             std::to_string(dspace.getSimpleExtentNdims()) +
                             ") do not match the field being read into (" +
                             std::to_string(N) + ").");
  hsize_t ddims[N];
  dspace.getSimpleExtentDims(ddims);

  std::array<size_t, N> dims;
  for (size_t i = 0; i < N; ++i) dims[i] = ddims[i];

  f = Field<FT, N, CT>(dims);

  dset.read(f.data(), detail::get_hdf5_dtype_for_type<FT>());

  for (size_t i = 0; i < N; ++i) {
    std::string dsetname{"axis " + std::to_string(i)};
    auto dset = container.openDataSet(dsetname.c_str());
    auto dspace = dset.getSpace();
    if (dspace.getSimpleExtentNdims() != 1)
      throw std::runtime_error("Cannot read axis data from '"+dsetname+"'. It does not contain a 1D array.");
    hsize_t ddims[1];
    dspace.getSimpleExtentDims(ddims);
    if (ddims[0] != f.size(i) )
      throw std::runtime_error("Cannot read axis data from '"+dsetname+"'. Size ("+std::to_string(ddims[0])+") does not match size expected from field data ("+std::to_string(f.size(i))+").");

    dset.read(f.getAxis(i).data(), detail::get_hdf5_dtype_for_type<CT>());
  }
}

/*
 * Reads a field from an HDF5 group specified by a list of path elements in an HDF5 container (either a group, or a file).
 *
 * @param container  the HDF5 container (file or group) to read from.
 * @param path a list of path elements to locate the group.
 * @param f the field to read data into.
 *
 * This function can be used to read fields from a file that contains multiple fields. It assumes
 * that the data is foratted the same way that it will be written. Each coordinate axis is read from
 * a dataset named "axis <N>", where N is the axis index (zero offset), and the field data is read
 * from a dataset named "field".
 */
template <typename ST, typename FT, size_t N, typename CT>
auto hdf5read(ST& container, const std::vector<std::string>& path, Field<FT, N, CT>& f) -> decltype(container.createGroup(std::string()),void())
{
  H5::Group group = container.openGroup("/");
  for( auto &elem: path )
  {
    group = group.openGroup(elem);
  }
  hdf5read(group,f);
}

/*
 * Reads a field from an HDF5 group specified by a path in an HDF5 container (either a group, or a file).
 *
 * @param container  the HDF5 container (file or group) to read from.
 * @param path a string representation of a path (i.e. '/path/to/element')
 * @param f the field to read data into.
 *
 * This function can be used to read fields from a file that contains multiple fields. It assumes
 * that the data is foratted the same way that it will be written. Each coordinate axis is read from
 * a dataset named "axis <N>", where N is the axis index (zero offset), and the field data is read
 * from a dataset named "field".
 */
template <typename ST, typename FT, size_t N, typename CT>
auto hdf5read(ST& container, std::string path, Field<FT, N, CT>& f) -> decltype(container.createGroup(std::string()),void())
{
  auto is_slash = [](char c){return c=='/';};
  boost::trim_if(path, is_slash);
  std::vector<std::string> elems;
  boost::split(elems, path, is_slash);
  hdf5read(container,elems,f);
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

/**
 * Reads a field from a specified group in an HdF5 file.
 *
 * This function assumes that the file contains a group (which is specified) that
 * is structured in the way written by hdf5write.
 */
template <typename FT, size_t N, typename CT>
void hdf5read(std::string name, std::string path, Field<FT, N, CT>& f)
{
  H5::H5File file(name.c_str(), H5F_ACC_RDONLY);
  try {
  hdf5read(file, path, f);
  } catch( std::runtime_error& e)
  {
    throw std::runtime_error("There was an error reading field from '" + name +". "+e.what());
  }

  file.close();
}

