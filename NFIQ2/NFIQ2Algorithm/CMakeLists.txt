cmake_minimum_required (VERSION 3.3)

project( nfiq2 )

set (CMAKE_CXX_STANDARD 11)

include( "${SUPERBUILD_ROOT_PATH}/cmake/colors.cmake" )
include( "${SUPERBUILD_ROOT_PATH}/cmake/target.cmake" )
include( "${SUPERBUILD_ROOT_PATH}/cmake/compiler.cmake" )

# add include directories
include_directories("${CMAKE_BINARY_DIR}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/features")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/prediction")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/wsq")

include_directories("${SUPERBUILD_ROOT_PATH}/fingerjetfxose/FingerJetFXOSE/libFRFXLL/include")

include_directories("${SUPERBUILD_ROOT_PATH}/digestpp")

include_directories("${SUPERBUILD_ROOT_PATH}/biomdi/biomdi/common/src/include")
include_directories("${SUPERBUILD_ROOT_PATH}/biomdi/biomdi/fingerminutia/src/include")

set( OpenCV_DIR ${CMAKE_BINARY_DIR}/../../../OpenCV-prefix/src/OpenCV-build)
find_package(OpenCV REQUIRED NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH HINTS ${OpenCV_DIR})
set(OpenCV_SHARED ON)
include_directories(${OpenCV_INCLUDE_DIRS})

if("${TARGET_PLATFORM}" MATCHES "win*")
  include_directories("${SUPERBUILD_ROOT_PATH}/biomdi/windows")
endif()

# FIXME: Change to "${CMAKE_INSTALL_PREFIX}/lib" once biomdi and FJFX builds 
# FIXME: are updated.
link_directories( "${CMAKE_INSTALL_PREFIX}" )

set(SOURCE_FILES "")
file( GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" )
list( REMOVE_ITEM SOURCE_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/NFIQ2.cpp"
)

file( GLOB FEATURES_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/features/*.cpp" )
file( GLOB PREDICTION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/prediction/*.cpp" )
file( GLOB WSQ_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/wsq/*.cpp" )

set(PUBLIC_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/NFIQ2Algorithm.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/InterfaceDefinitions.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/FingerprintImageData.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Data.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/NFIQException.h"
)

add_library( ${PROJECT_NAME} STATIC
  ${SOURCE_FILES}
  ${FEATURES_FILES}
  ${PREDICTION_FILES}
  ${WSQ_FILES}
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    PUBLIC_HEADER "${PUBLIC_HEADERS}"
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
  zlib
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
  add_definitions("-DNOMINMAX")
  if( "${OPENCV_VERSION}" MATCHES "^3.*")
    set( PROJECT_LIBS ${PROJECT_LIBS}
      ws2_32
    )
  else()
    set( PROJECT_LIBS ${PROJECT_LIBS}
      ws2_32
    )
  endif()
elseif("${TARGET_PLATFORM}" MATCHES "android*")
  find_package(Threads REQUIRED)
  set( PROJECT_LIBS ${PROJECT_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
    log
    ${CMAKE_DL_LIBS}
  )
else()
  find_package(Threads REQUIRED)
  set( PROJECT_LIBS ${PROJECT_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_DL_LIBS}
  )
endif()

target_link_libraries( ${NFIQ2_TEST_APP} ${PROJECT_LIBS} ${OpenCV_LIBS} )

install(TARGETS ${NFIQ2_TEST_APP} 
    RUNTIME DESTINATION bin)
install(TARGETS ${PROJECT_NAME} 
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include)
install(FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/../nfiq2rf.yaml"
    DESTINATION bin)


