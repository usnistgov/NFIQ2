project( opencv ) 

# download sources
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/download/)
  set( OPENCV_SRC "https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.zip")
  set( OPENCV_DST "${CMAKE_CURRENT_SOURCE_DIR}/download/tmp.zip")
  message("Download ${OPENCV_SRC} to ${OPENCV_DST}")
  file(DOWNLOAD "${OPENCV_SRC}" "${OPENCV_DST}")
  file(SIZE "${OPENCV_DST}" OPENCV_SIZE)
  message("Downloaded ${OPENCV_SIZE} bytes")
  execute_process( COMMAND cmake -E tar xfz tmp.zip WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/download/ )
  file(REMOVE "${OPENCV_DST}")
endif()

#tweak opencv build
include( "${CMAKE_CURRENT_SOURCE_DIR}/download/opencv-${OPENCV_VERSION}/cmake/OpenCVUtils.cmake" )

OCV_OPTION(BUILD_SHARED_LIBS        "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)"  OFF )
OCV_OPTION(BUILD_opencv_apps        "Build utility applications (used for example to train classifiers)"  OFF )
OCV_OPTION(BUILD_ANDROID_EXAMPLES   "Build examples for Android platform"                                 OFF )
OCV_OPTION(BUILD_DOCS               "Create build rules for OpenCV Documentation"                         OFF )
OCV_OPTION(BUILD_EXAMPLES           "Build all examples"                                                  OFF )
OCV_OPTION(BUILD_PACKAGE            "Enables 'make package_source' command"                               OFF )
OCV_OPTION(BUILD_PERF_TESTS         "Build performance tests"                                             OFF )
OCV_OPTION(BUILD_TESTS              "Build accuracy & regression tests"                                   OFF )
OCV_OPTION(BUILD_WITH_DEBUG_INFO    "Include debug info into debug libs (not MSCV only)"                  OFF )
OCV_OPTION(BUILD_FAT_JAVA_LIB       "Create fat java wrapper containing the whole OpenCV library"         OFF )
OCV_OPTION(BUILD_ANDROID_SERVICE    "Build OpenCV Manager for Google Play"                                OFF )
OCV_OPTION(BUILD_ANDROID_PACKAGE    "Build platform-specific package for Google Play"                     OFF )
OCV_OPTION(BUILD_TINY_GPU_MODULE    "Build tiny gpu module with limited image format support"             OFF )
OCV_OPTION(ENABLE_PRECOMPILED_HEADERS "Use precompiled headers"                                           OFF )
OCV_OPTION(WITH_GTK                 "Include GTK support"                                                 OFF )
OCV_OPTION(ENABLE_FAST_MATH         "Enable -ffast-math (not recommended for GCC 4.6.x)"                  OFF )
OCV_OPTION(BUILD_WITH_STATIC_CRT    "Enables use of statically linked CRT for staticaly linked OpenCV"    ON  )
OCV_OPTION(BUILD_ZLIB               "Build zlib from source"                                              ON  )

# opencv 3 options
# disable protobuf, due it requires CPP11 build
OCV_OPTION(WITH_PROTOBUF            "Enable libprotobuf"                                                  OFF )
OCV_OPTION(BUILD_PROTOBUF           "Force to build libprotobuf from sources"                             OFF )
OCV_OPTION(WITH_IPP                 "Include Intel IPP support"                                           OFF )
OCV_OPTION(WITH_ITT                 "Include Intel ITT support"                                           OFF )
OCV_OPTION(WITH_DSHOW               "Build VideoIO with DirectShow support"                               OFF )
OCV_OPTION(WITH_MSMF                "Build VideoIO with Media Foundation support"                         OFF )

set(BUILD_LIST "core,ml,imgproc" CACHE STRING "Build only listed modules (comma-separated, e.g. 'videoio,dnn,ts')")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/download/opencv-${OPENCV_VERSION}" "${BUILD_PATH}/opencv-${OPENCV_VERSION}/")
