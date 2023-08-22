#! /bin/bash

ROOT=$(git rev-parse --show-toplevel)

cd $ROOT
rm build -rf
conan install . -s build_type=Release
cmake --preset conan-release -DUNIT_TESTS=OFF
cmake --build --preset conan-release
cmake --install build/Release --prefix build/INSTALL

cd build
cmake ../testing/client_project -DCMAKE_PREFIX_PATH=$PWD/INSTALL
cmake --build . --config Release
./Release/example

