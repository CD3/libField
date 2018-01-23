cmake_minimum_required( VERSION 3.1 )

OPTION( BUILD_UNIT_TESTS "Build unit tests for the library" ON )


if(BUILD_UNIT_TESTS)

set( binDir ${CMAKE_BINARY_DIR}/testBin )
set( buildDir ${CMAKE_BINARY_DIR} )

# Find all sources.
file( GLOB_RECURSE unitTest_SOURCES
      RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
      "./CatchTests/*.cpp" )
message(STATUS "Detected Catch-based Unit Test Sources:")
foreach(testSrc ${unitTest_SOURCES})
  message(STATUS "  ${testSrc}" )
endforeach()

set( testName ${PROJECT_NAME}_CatchTests )
add_executable(${testName} ${unitTest_SOURCES})
target_compile_definitions( ${testName} PRIVATE -DTESTING )
target_link_libraries(${testName} Field)
target_include_directories( ${testName} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include" )
set_target_properties(${testName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${binDir})
set_target_properties(${testName} PROPERTIES CXX_STANDARD 14)

add_test(NAME ${testName} 
         WORKING_DIRECTORY ${buildDir}
         COMMAND ${binDir}/${testName} )

file(COPY data DESTINATION ${buildDir} )

endif()



## _   _             _             ____                  _                          _        
##| \ | | ___  _ __ (_)_   _ ___  | __ )  ___ _ __   ___| |__  _ __ ___   __ _ _ __| | _____ 
##|  \| |/ _ \| '_ \| | | | / __| |  _ \ / _ \ '_ \ / __| '_ \| '_ ` _ \ / _` | '__| |/ / __|
##| |\  | (_) | | | | | |_| \__ \ | |_) |  __/ | | | (__| | | | | | | | | (_| | |  |   <\__ \
##|_| \_|\___/|_| |_|_|\__,_|___/ |____/ \___|_| |_|\___|_| |_|_| |_| |_|\__,_|_|  |_|\_\___/
                                                                                           

#set( binDir ${CMAKE_BINARY_DIR}/benchmarkBin )

## Benchmarks written with the Nonius framework are
## automatically detected. Just put the source files
## in the `NoniusBenchmarks` directory.

#set( benchmark_DIR ./NoniusBenchmarks)
## Search for sources.
#file( GLOB_RECURSE benchmark_SOURCES
      #RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
      #"${benchmark_DIR}/*.cpp" )
    #message(STATUS "Detected Nonius-based Benchmark Sources:")
#foreach(benchmarkSrc ${benchmark_SOURCES})
  #message(STATUS "  ${benchmarkSrc}" )
#endforeach()

## Nonius requires a threading library
#find_package(Threads)

#if(Threads_FOUND)

## Add benchmark executable
#set( benchmarkName NoniusBenchmarks )
#add_executable(${benchmarkName} ${benchmark_SOURCES})
##target_compile_definitions( ${benchmarkName} )

## Link dependencies
#target_link_libraries(${benchmarkName} Threads::Threads ) # If your project builds a library, link to it here

## Make sure that testing binaries are created in the binDir directory and use C++11
#set_target_properties(${benchmarkName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${binDir} CXX_STANDARD 14)


#endif()


