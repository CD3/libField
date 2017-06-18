cmake_minimum_required( VERSION 2.8 )

add_subdirectory(libField)
include_directories(${libField_INCLUDE_DIRS})
set( CMAKE_CXX_STANDARD 14 )
add_executable( demo-libField demo-libField.cpp )
add_executable( demo-staticArray demo-staticArray.cpp )
add_executable( demo-dynamicArray demo-dynamicArray.cpp )
add_executable( demo-dynamicArray2 demo-dynamicArray2.cpp )

