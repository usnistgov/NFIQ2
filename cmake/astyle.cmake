set( ASTYLE_EXE "${DIST_PATH}/astyle${CMAKE_EXECUTABLE_SUFFIX}" )

if( NOT ANDROID AND NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # Error	C1061	compiler limit: blocks nested too deeply	astyle	D:\Devel\ISO\NFIQ2-official\NFIQ2-rlessmann\astyle\src\astyle_main.cpp	3702	
  add_subdirectory("${ROOT_PATH}/astyle/" "${BUILD_PATH}/astyle/")
  # add project folders to be "astyled"
  file(GLOB ASTYLE_FILES "${ROOT_PATH}/NFIQ2/NFIQ2Api/*.cpp" "${ROOT_PATH}/NFIQ2/NFIQ2Api/*.h" ) 
  add_custom_target( format
    COMMAND ${ASTYLE_EXE} --options=${ROOT_PATH}/astyle.cfg ${ASTYLE_FILES} 
    DEPENDS astyle
  )
else()
  add_custom_target( astyle
    COMMAND echo "Skip build target 'astyle'"
  )
  add_custom_target( format
    COMMAND echo "Skip build target 'format'"
    DEPENDS astyle
  )
endif()



