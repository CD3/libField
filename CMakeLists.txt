cmake_minimum_required(VERSION 3.14 )

if(TARGET libField)
  return()
endif(TARGET libField)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  set(STANDALONE ON)
endif()

enable_testing()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(
  SERIALIZATION
  "Enalbe Boost serialization. This requires the Boost.Serialization library, and defining BOOST_NO_MEMBER_TEMPLATE_FREINDS."
  OFF)

project(libField)
include(macro-git_version)
git_version(${PROJECT_NAME})
message(STATUS "${PROJECT_NAME} version: ${${PROJECT_NAME}_VERSION_FULL}")

# need to run configure_file twice
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}/version.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}_version.h")
configure_file("${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}_version.h"
  "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}_version.h")

find_package(Boost REQUIRED)
if(SERIALIZATION)
  find_package(Boost REQUIRED COMPONENTS serialization)
endif()
find_package( hdf5 QUIET )
if( TARGET hdf5::hdf5_cpp-static )
  message( STATUS "Found HDF5 C++ Library" )
else()
  message( STATUS "Did not find HDF5 C++ Library" )
endif()
find_package( gputils QUIET )
if( TARGET gputils::gputils )
  message( STATUS "Found gp-utils C++ Library" )
else()
  message( STATUS "Did not find gp-utils C++ Library" )
endif()


string(REGEX
  REPLACE "^lib"
  ""
  LIB_NAME
  ${PROJECT_NAME})
add_library(${LIB_NAME} INTERFACE)
target_include_directories(
  ${LIB_NAME}
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(${LIB_NAME} INTERFACE cxx_std_14)
target_sources(${LIB_NAME} INTERFACE)

target_link_libraries(
  ${LIB_NAME}
  INTERFACE Boost::boost $<$<BOOL:${SERIALIZATION}>:Boost::serialization>)

target_compile_definitions(${LIB_NAME}
  INTERFACE
  $<$<BOOL:${SERIALIZATION}>:SERIALIZATION_ENABLED
  BOOST_NO_MEMBER_TEMPLATE_FRIENDS>)
target_compile_definitions( ${LIB_NAME} INTERFACE $<$<TARGET_EXISTS:hdf5::hdf5_cpp-static>:HAVE_HDF5_CPP> $<$<TARGET_EXISTS:gputils::gputils>:HAVE_GPUTILS>)
target_link_libraries( ${LIB_NAME} INTERFACE $<$<TARGET_EXISTS:hdf5::hdf5_cpp-static>:hdf5::hdf5_cpp-static>
                                             $<$<TARGET_EXISTS:gputils::gputils>:gputils::gputils> )

if(STANDALONE)
  add_subdirectory(testing)
  add_subdirectory(doc)
endif()

install(TARGETS ${LIB_NAME}
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include)
install(DIRECTORY src/
  DESTINATION include/
  FILES_MATCHING
  PATTERN "*.hpp"
  PATTERN "*.h")
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/
  DESTINATION include/${PROJECT_NAME}
  FILES_MATCHING
  PATTERN "*.hpp"
  PATTERN "*.h")
install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION cmake)

# we need to conditionally add hdf5 dependency
# if hdf5::hdf5_cpp-static target exists, then libField
# will link against it and the user needs to have it installed.
if(TARGET hdf5::hdf5_cpp-static)
set( HDF5_DEP "find_dependency(hdf5)")
elseif()
set( HDF5_DEP "")
endif()
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  "include(CMakeFindDependencyMacro)
  find_dependency(Boost)
  ${HDF5_DEP}
  include(\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}Targets.cmake)
  ")
  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${${PROJECT_NAME}_VERSION}
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
    )
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION cmake)

  include(InstallRequiredSystemLibraries)

  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "A library for doing computations on a discretized field.")
  set(CPACK_PACKAGE_VENDOR "C.D. Clark III")
  set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md")
  if(${PROJECT_NAME}_VERSION_MAJOR)
    set(CPACK_PACKAGE_VERSION_MAJOR ${${PROJECT_NAME}_VERSION_MAJOR})
  endif()
  if(${PROJECT_NAME}_VERSION_MINOR)
    set(CPACK_PACKAGE_VERSION_MINOR ${${PROJECT_NAME}_VERSION_MINOR})
  endif()
  if(${PROJECT_NAME}_VERSION_PATCH)
    set(CPACK_PACKAGE_VERSION_PATCH ${${PROJECT_NAME}_VERSION_PATCH})
  endif()
  include(CPack)
