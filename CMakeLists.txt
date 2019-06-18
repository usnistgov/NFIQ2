cmake_minimum_required (VERSION 3.3)

project( NFIQ2 )

set( NO_SEARCH TRUE )

set( ROOT_PATH ${PROJECT_SOURCE_DIR} )
set( BUILD_PATH  ${CMAKE_CURRENT_BINARY_DIR} )

set( OPENCV_VERSION "2.4.13.6")
#set( OPENCV_VERSION "3.4.5")

# set colors for cmake comand line
include( "${ROOT_PATH}/cmake/colors.cmake" )

# detect target platform
include( "${ROOT_PATH}/cmake/target.cmake" )

# setup compiler
include( "${ROOT_PATH}/cmake/compiler.cmake" )

# create build and distribution path
include( "${ROOT_PATH}/cmake/folders.cmake" )

# include special settings for fingerjetfxose
include( "${ROOT_PATH}/cmake/fingerjetfxose.cmake" )

message( STATUS "----- Configuring ${PROJECT_NAME} -----" ) 
message( STATUS "-- Project root path: '${ROOT_PATH}'" )
message( STATUS "-- Project build path: '${BUILD_PATH}'" )
message( STATUS "-- Project distribution path: '${DIST_PATH}'" )

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DIST_PATH} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DIST_PATH} )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DIST_PATH} )

add_subdirectory("${ROOT_PATH}/biomdi/" "${BUILD_PATH}/biomdi/")
add_subdirectory("${ROOT_PATH}/fingerjetfxose/FingerJetFXOSE/libFRFXLL/src/" "${BUILD_PATH}/fingerjetfxose/FingerJetFXOSE/libFRFXLL/src/")
add_subdirectory("${ROOT_PATH}/opencv/" "${BUILD_PATH}/opencv/")
add_subdirectory("${ROOT_PATH}/nfiq2/NFIQ2Algorithm" "${BUILD_PATH}/nfiq2/NFIQ2Algorithm")
add_subdirectory("${ROOT_PATH}/nfiq2/NFIQ2Api" "${BUILD_PATH}/nfiq2/NFIQ2Api")
