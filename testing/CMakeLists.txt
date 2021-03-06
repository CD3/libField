cmake_minimum_required(VERSION 3.1)

option(BUILD_UNIT_TESTS "Build unit tests for the library" ON)

if(BUILD_UNIT_TESTS)

  set(binDir ${CMAKE_BINARY_DIR}/testBin)
  set(buildDir ${CMAKE_BINARY_DIR})

  find_package(OpenMP)
  find_package(libIntegrate)

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
  if(MSVC)
    target_compile_options(${testName} PRIVATE /W4 /WX)
  else()
    target_compile_options(${testName} PRIVATE -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-parameter -pedantic -Werror)
  endif()

  target_link_libraries(${testName} Field )
  if(TARGET libIntegrate::Integrate)
    target_compile_definitions(${testName} PRIVATE HAVE_LIBINTEGRATE)
    target_link_libraries(${testName} libIntegrate::Integrate)
  endif()
  target_link_libraries(${testName} Field )
  if(OpenMP_CXX_FOUND)
    target_link_libraries(${testName} OpenMP::OpenMP_CXX)
  endif()
  target_include_directories(${testName}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
  set_target_properties(${testName}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${binDir})

  add_test(NAME ${testName}
    WORKING_DIRECTORY ${buildDir}
    COMMAND ${binDir}/${testName})

endif()
