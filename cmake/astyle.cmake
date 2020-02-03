# set astyle executable
set( ASTYLE_EXE "${DIST_PATH}/astyle${CMAKE_EXECUTABLE_SUFFIX}" )

# add project folders to be "astyled"
file(GLOB ASTYLE_NFIQ2_ALGORITHM "${ROOT_PATH}/NFIQ2/NFIQ2Algorithm/*.cpp" "${ROOT_PATH}/NFIQ2/NFIQ2Algorithm/*.h" ) 
file(GLOB ASTYLE_NFIQ2_API "${ROOT_PATH}/NFIQ2/NFIQ2Api/*.cpp" "${ROOT_PATH}/NFIQ2/NFIQ2Api/*.h" ) 
file(GLOB ASTYLE_NFIQ2_TRAINING "${ROOT_PATH}/NFIQ2/NFIQ2Training/*.cpp" "${ROOT_PATH}/NFIQ2/NFIQ2Training/*.h" ) 

add_custom_target( format
  COMMAND ${ASTYLE_EXE} --options=${ROOT_PATH}/astyle.cfg ${ASTYLE_NFIQ2_ALGORITHM} ${ASTYLE_NFIQ2_API} ${ASTYLE_NFIQ2_TRAINING}
)



