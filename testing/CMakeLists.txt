
find_package(OpenMP)
find_package(Catch2 REQUIRED)
find_package(HDF5 REQUIRED)

# Find all sources.
file(GLOB_RECURSE unitTest_SOURCES
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "./CatchTests/*.cpp")
message(STATUS "Detected Catch-based Unit Test Sources:")
foreach(testSrc ${unitTest_SOURCES})
  message(STATUS "  ${testSrc}")
endforeach()

set(testName ${PROJECT_NAME}_CatchTests)
add_executable(${testName} ${unitTest_SOURCES})

target_compile_definitions(${testName} PRIVATE -DTESTING -DHAVE_HDF5_CPP)
if(MSVC)
  target_compile_options(${testName} PRIVATE /W4 /WX)
else()
  target_compile_options(${testName} PRIVATE -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-parameter -pedantic -Werror)
endif()

target_link_libraries(${testName} Field Catch2::Catch2WithMain hdf5::hdf5_cpp)
if(OpenMP_CXX_FOUND)
  target_link_libraries(${testName} OpenMP::OpenMP_CXX)
endif()

add_test(NAME ${testName}
  WORKING_DIRECTORY ${buildDir}
  COMMAND ${binDir}/${testName})

