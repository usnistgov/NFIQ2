project( Nfiq2Api ) 

add_definitions( "-std=c++11" )

# add files from folder
file( GLOB SOURCE_FILES "nfiq2*.cpp" ) 
file( GLOB HEADER_FILES "nfiq2*.h" ) 

# add include directories
include_directories("${CMAKE_BINARY_DIR}")

include_directories("${ROOT_PATH}/NFIQ2/NFIQ2Algorithm")
include_directories("${ROOT_PATH}/NFIQ2/NFIQ2Algorithm/include")
include_directories("${ROOT_PATH}/NFIQ2/NFIQ2Algorithm/include/features")
include_directories("${ROOT_PATH}/NFIQ2/NFIQ2Algorithm/include/prediction")
include_directories("${ROOT_PATH}/NFIQ2/NFIQ2Algorithm/include/wsq")

include_directories("${ROOT_PATH}/fingerjetfxose/download/FingerJetFXOSE-master/src/libFRFXLL/include")

include_directories("${ROOT_PATH}/biomdi/biomdi/common/src/include")
include_directories("${ROOT_PATH}/biomdi/biomdi/fingerminutia/src/include")

include_directories("${ROOT_PATH}/opencv/download/opencv-${OPENCV_VERSION}/include")
include_directories("${ROOT_PATH}/opencv/download/opencv-${OPENCV_VERSION}/modules/core/include")
include_directories("${ROOT_PATH}/opencv/download/opencv-${OPENCV_VERSION}/modules/imgproc/include")
include_directories("${ROOT_PATH}/opencv/download/opencv-${OPENCV_VERSION}/modules/highgui/include")
include_directories("${ROOT_PATH}/opencv/download/opencv-${OPENCV_VERSION}/modules/video/include")
include_directories("${ROOT_PATH}/opencv/download/opencv-${OPENCV_VERSION}/modules/features2d/include")
include_directories("${ROOT_PATH}/opencv/download/opencv-${OPENCV_VERSION}/modules/flann/include")
include_directories("${ROOT_PATH}/opencv/download/opencv-${OPENCV_VERSION}/modules/calib3d/include")
include_directories("${ROOT_PATH}/opencv/download/opencv-${OPENCV_VERSION}/modules/objdetect/include")
include_directories("${ROOT_PATH}/opencv/download/opencv-${OPENCV_VERSION}/modules/legacy/include")
include_directories("${ROOT_PATH}/opencv/download/opencv-${OPENCV_VERSION}/modules/ml/include")
# only OpenCV3
include_directories("${ROOT_PATH}/opencv/download/opencv-${OPENCV_VERSION}/modules/imgcodecs/include")
include_directories("${ROOT_PATH}/opencv/download/opencv-${OPENCV_VERSION}/modules/videoio/include")
include_directories("${ROOT_PATH}/opencv/download/opencv-${OPENCV_VERSION}/modules/photo/include")

if("${TARGET_PLATFORM}" MATCHES "win*")
  include_directories("${ROOT_PATH}/biomdi/deps/windows")
endif()

# set and increment version
include( "${ROOT_PATH}/NFIQ2/version.cmake" )

# add targets
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_library( ${PROJECT_NAME} SHARED
      "nfiq2api.def"
      ${SOURCE_FILES}
      ${VERSION_FILES}
    )
else()
    add_library( ${PROJECT_NAME} SHARED
      ${SOURCE_FILES}
      ${VERSION_FILES}
    )
endif()

string( REPLACE "." "" OPENCV_VERSION2 "${OPENCV_VERSION}")

if("${TARGET_PLATFORM}" MATCHES "win*")
  # remove the lib prefix for Windows
  set_target_properties( ${PROJECT_NAME} PROPERTIES PREFIX "" )
endif()

add_dependencies( ${PROJECT_NAME}
  opencv_core
  opencv_ml
  opencv_imgproc
)

# add required link libraries
set( PROJECT_LIBS
  nfiq2
  FRFXLL_static
  fmr
  biomdi
)

if( USE_SANITIZER )
  target_link_libraries( ${PROJECT_NAME} "asan" ) 
endif()


if("${TARGET_PLATFORM}" MATCHES "win*")
  if( "${OPENCV_VERSION}" MATCHES "^3.*")
    set( PROJECT_LIBS ${PROJECT_LIBS}
      ${LINK_LIB_PREFIX}opencv_ml${OPENCV_VERSION2}${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${LINK_LIB_PREFIX}opencv_imgproc${OPENCV_VERSION2}${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${LINK_LIB_PREFIX}opencv_core${OPENCV_VERSION2}${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${LINK_3RD_PREFIX}zlib${CMAKE_STATIC_LIBRARY_SUFFIX}
      ws2_32
    )
  else()
    set( PROJECT_LIBS ${PROJECT_LIBS}
      ${LINK_LIB_PREFIX}opencv_ml2413${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${LINK_LIB_PREFIX}opencv_imgproc2413${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${LINK_LIB_PREFIX}opencv_core2413${CMAKE_STATIC_LIBRARY_SUFFIX}
      ${LINK_3RD_PREFIX}zlib${CMAKE_STATIC_LIBRARY_SUFFIX}
      ws2_32
    )
  endif()
elseif("${TARGET_PLATFORM}" MATCHES "android*")
  find_package(Threads REQUIRED)
  set( PROJECT_LIBS ${PROJECT_LIBS}
    ${LINK_LIB_PREFIX}opencv_ml${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${LINK_LIB_PREFIX}opencv_imgproc${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${LINK_LIB_PREFIX}opencv_core${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${LINK_3RD_PREFIX}zlib${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${CMAKE_THREAD_LIBS_INIT}
    log
  )
else()
  find_package(Threads REQUIRED)
  set( PROJECT_LIBS ${PROJECT_LIBS}
    ${LINK_LIB_PREFIX}opencv_ml${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${LINK_LIB_PREFIX}opencv_imgproc${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${LINK_LIB_PREFIX}opencv_core${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${LINK_3RD_PREFIX}zlib${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${CMAKE_THREAD_LIBS_INIT}
    dl
  )
endif()

link_directories( "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )

target_link_libraries( "${PROJECT_NAME}" ${PROJECT_LIBS} ) 
