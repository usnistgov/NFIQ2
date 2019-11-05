project( nfiq2 ) 

add_definitions( "-std=c++11" )

# add include directories
include_directories("${CMAKE_BINARY_DIR}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/features")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/prediction")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/wsq")

include_directories("${FRFXLL_PATH}/include")

include_directories("${ROOT_PATH}/digestpp")

include_directories("${ROOT_PATH}/biomdi/biomdi/common/src/include")
include_directories("${ROOT_PATH}/biomdi/biomdi/fingerminutia/src/include")

find_package(OpenCV REQUIRED NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH HINTS ${CMAKE_BINARY_DIR})
set(OpenCV_SHARED ON)
include_directories(${OpenCV_INCLUDE_DIRS})

if("${TARGET_PLATFORM}" MATCHES "win*")
  include_directories("${ROOT_PATH}/biomdi/windows")
endif()

set(SOURCE_FILES "")
file( GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" ) 
list( REMOVE_ITEM SOURCE_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/NFIQ2.cpp"
)

file( GLOB FEATURES_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/features/*.cpp" ) 
file( GLOB PREDICTION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/prediction/*.cpp" ) 
file( GLOB WSQ_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/wsq/*.cpp" ) 

add_library( ${PROJECT_NAME} STATIC
  ${SOURCE_FILES}
  ${FEATURES_FILES}
  ${PREDICTION_FILES}
  ${WSQ_FILES}
)

#add the test program
string( REPLACE "." "" OPENCV_VERSION2 "${OPENCV_VERSION}")
string( REPLACE "." "" FRFXLL_VERSION2 "${FRFXLL_VERSION}")
set( NFIQ2_TEST_APP "${PROJECT_NAME}.${CMAKE_CXX_COMPILER_ID}.${TARGET_PLATFORM}.ocv${OPENCV_VERSION2}.fjfx${FRFXLL_VERSION2}.test")

add_executable( ${NFIQ2_TEST_APP}
  "${CMAKE_CURRENT_SOURCE_DIR}/src/NFIQ2.cpp"
)

add_dependencies( ${NFIQ2_TEST_APP}
  opencv_core
  opencv_ml
  opencv_imgproc
  opencv_highgui
  opencv_flann
  opencv_features2d
  opencv_calib3d
  opencv_video
  opencv_objdetect
  opencv_photo
  opencv_stitching
  opencv_videostab
)

if( USE_SANITIZER )
  target_link_libraries( ${NFIQ2_TEST_APP} "asan" ) 
endif()

# set the required libraries
set( PROJECT_LIBS
  nfiq2
  FRFXLL_static
  fmr
  biomdi
)

if("${TARGET_PLATFORM}" MATCHES "win*")
  if( "${OPENCV_VERSION}" MATCHES "^3.*")
    set( PROJECT_LIBS ${PROJECT_LIBS}
      ${LINK_3RD_PREFIX}zlib${CMAKE_STATIC_LIBRARY_SUFFIX}
      ws2_32
    )
  else()
    set( PROJECT_LIBS ${PROJECT_LIBS}
      ${LINK_3RD_PREFIX}zlib${CMAKE_STATIC_LIBRARY_SUFFIX}
      ws2_32
    )
  endif()
elseif("${TARGET_PLATFORM}" MATCHES "android*")
  find_package(Threads REQUIRED)
  set( PROJECT_LIBS ${PROJECT_LIBS}
    ${LINK_3RD_PREFIX}zlib${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${CMAKE_THREAD_LIBS_INIT}
    log
	dl
  )
else()
  find_package(Threads REQUIRED)
  set( PROJECT_LIBS ${PROJECT_LIBS}
    ${LINK_3RD_PREFIX}zlib${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${CMAKE_THREAD_LIBS_INIT}
	dl
  )
endif()

link_directories( "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )

target_link_libraries( ${NFIQ2_TEST_APP} ${PROJECT_LIBS} ${OpenCV_LIBS} )
