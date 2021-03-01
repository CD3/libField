#pragma once

/** @file IO.hpp
 * @brief Functions for reading and writing fields.
 * @author C.D. Clark III
 * @date 03/01/21
 */

#include <gputils/io.hpp>

#include "./Field.hpp"

template<typename FT, typename CT>
void asciiread(std::string filename, Field<FT, 1, CT>& f)
{
  GP2DData data;
  ReadGPASCII2DDataFile(filename, data);

  f.reset(data.x.size());
  for (size_t i = 0; i < f.size(); ++i) {
    f.getAxis(0)[i] = data.x[i];
    f(i)            = data.f[i];
  }
}

template<typename FT, typename CT>
void asciiwrite(std::string filename, Field<FT, 1, CT>& f)
{
  GP2DData data;
  for( size_t i = 0; i < f.size(); ++i)
  {
    data.x.push_back(f.getAxis(0)[i]);
    data.f.push_back(f(i));
  }
  WriteGPASCII2DDataFile(filename,data);
}

