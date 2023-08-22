#! /bin/bash

srcdir=$PWD
bindir=${srcdir}/.build-and-test

function cleanup ()
{
  [[ -d ${bindir} ]] && rm -r ${bindir}
}
set -e
trap cleanup EXIT

mkdir ${bindir}
cd ${bindir}
conan install ..
veval cmake .. -DCMAKE_INSTALL_PREFIX=${bindir}/install -DCMAKE_BUILD_TYPE=Release
cmake --build .
cmake --build . --target test








# test install

cmake --build . --target install

mkdir app
cd app

cat << EOF > main.cpp
#include <iostream>
#include <libField/libField_version.h>
#include <libField/Field.hpp>

int main()
{
  std::cout << "VERSION: " << libField_VERSION << std::endl;
  std::cout << "FULL VERSION: " << libField_VERSION_FULL << std::endl;

  Field<double,3> F(10,5,2);

  return 0;
}
EOF

cat << EOF > CMakeLists.txt
cmake_minimum_required(VERSION 3.1)
add_executable( main main.cpp )
find_package( libField REQUIRED PATHS ${bindir}/install )
target_link_libraries(main libField::Field )
set_target_properties(main PROPERTIES CXX_STANDARD 14)
EOF

mkdir build1
cd build1
conan install ${srcdir}
veval cmake .. -DlibField_DIR=${bindir}/install/cmake/libField
cmake --build .
./main

cd ..

mkdir build2
cd build2
conan install ${srcdir}
veval cmake ..
cmake --build .
./main

echo "PASSED"
