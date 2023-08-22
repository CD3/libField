cmake_minimum_required(VERSION 3.25)

set(libField_VERSION
    0.9.1
    CACHE STRING "Library version number")
set(libField_VERSION $CACHE{libField_VERSION})

project(libField VERSION ${libField_VERSION})

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(CMakePrintHelpers)


message(STATUS "libField Version: ${libField_VERSION}")

option(UNIT_TESTS "Build the unit tests" ON)


# Find and set up Boost
find_package(Boost REQUIRED COMPONENTS headers)

# Create the header-only library target
add_library(Field INTERFACE)
add_library(libField::Field ALIAS Field)

# Set include directories for the library
target_include_directories(Field
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Link the library against Boost headers
target_link_libraries(Field
    INTERFACE
        Boost::headers
)

if(UNIT_TESTS)
  add_subdirectory(testing)
endif()

# Specify installation rules for the library headers

install(
  DIRECTORY src/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING
  PATTERN *.hpp
  PATTERN *.h
  )

# Install the generated headers (i.e. version and export header)
install(
  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING
  PATTERN *.hpp
  PATTERN *.h
  )

install(TARGETS Field
    EXPORT libFieldTargets
)

install(EXPORT libFieldTargets
    FILE libFieldTargets.cmake
    NAMESPACE libField::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libField
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/libFieldConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

file(
    WRITE ${CMAKE_CURRENT_BINARY_DIR}/libFieldConfig.cmake
  "include(CMakeFindDependencyMacro)
  find_dependency(Boost REQUIRED COMPONENTS headers)
  include(\${CMAKE_CURRENT_LIST_DIR}/libFieldTargets.cmake)
")

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/libFieldConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/libFieldConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libField
)
