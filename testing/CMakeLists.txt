cmake_minimum_required(VERSION 3.1)

option(BUILD_UNIT_TESTS "Build unit tests for the library" ON)

find_package( hdf5 QUIET)

if(BUILD_UNIT_TESTS)

  set(binDir ${CMAKE_BINARY_DIR}/testBin)
  set(buildDir ${CMAKE_BINARY_DIR})

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

  target_compile_definitions(${testName} PRIVATE -DTESTING)
  target_link_libraries(${testName} Field)
  target_include_directories(${testName}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
  if( TARGET hdf5::hdf5_cpp-static)
    target_compile_definitions( ${testName} PRIVATE "HAVE_HDF5_CPP" )
    target_link_libraries( ${testName} hdf5::hdf5_cpp-static )
  endif()
  set_target_properties(${testName}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${binDir})

  add_test(NAME ${testName}
    WORKING_DIRECTORY ${buildDir}
    COMMAND ${binDir}/${testName})

  file(COPY data DESTINATION ${buildDir})

endif()
