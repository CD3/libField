find_package( Doxygen )

if( TARGET Doxygen::doxygen )
  set(DOXYGEN_EXTRACT_ALL OFF)
  set(DOXYGEN_EXTRACT_PRIVATE OFF)
  set(DOXYGEN_EXTRACT_STATIC OFF)
  set(DOXYGEN_HIDE_UNDOC_MEMBERS ON)
  set(DOXYGEN_HIDE_UNDOC_CLASSS ON)
  doxygen_add_docs( docs
    ${CMAKE_SOURCE_DIR}/src/
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()
