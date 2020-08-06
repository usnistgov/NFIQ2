cmake_minimum_required (VERSION 3.3)

project( nfiq2 )

set (CMAKE_CXX_STANDARD 11)

if (CMAKE_VERSION VERSION_GREATER 3.14.9999) 
    cmake_policy(SET CMP0091 NEW) 
endif(CMAKE_VERSION VERSION_GREATER 3.14.9999)

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

# add NFIQ2 Refresh tool include directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/tool")

# add libbiomeval include directories
include_directories("${CMAKE_SOURCE_DIR}/../../libbiomeval/src/include")

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
file( GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
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

# New NFIQ2 Refresh App
set( NFIQ2_TEST_APP "nfiq2-bin")

add_executable(${NFIQ2_TEST_APP})
target_sources(${NFIQ2_TEST_APP} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/tool/nfiq2_ui_refresh.cpp)
target_sources(${NFIQ2_TEST_APP} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/tool/nfiq2_ui_log.cpp)
target_sources(${NFIQ2_TEST_APP} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/tool/nfiq2_ui_utils.cpp)
target_sources(${NFIQ2_TEST_APP} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/tool/nfiq2_ui_exception.cpp)
target_sources(${NFIQ2_TEST_APP} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/tool/nfiq2_ui_threadedlog.cpp)
target_sources(${NFIQ2_TEST_APP} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/tool/nfiq2_ui_image.cpp)
target_sources(${NFIQ2_TEST_APP} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/tool/nfiq2_ui_types.cpp)

# Getopt for windows via VCPKG
if(MSVC)
  find_library(GETOPT_LIB getopt REQUIRED)
  find_file(GETOPT_HEADER getopt.h REQUIRED)
  get_filename_component(GETOPT_HEADER_DIR "${GETOPT_HEADER}" DIRECTORY)
  target_include_directories(${NFIQ2_TEST_APP} PRIVATE "${GETOPT_HEADER_DIR}")
  target_link_libraries(${NFIQ2_TEST_APP} "${GETOPT_LIB}")
endif()

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
)

#find_package(zlib REQUIRED)
#target_link_libraries( ${NFIQ2_TEST_APP} ${ZLIB_LIBRARIES})

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

# TODO: libbiomeval should implement find_package
list(APPEND CMAKE_MODULE_PATH "${SUPERBUILD_ROOT_PATH}/libbiomeval/src/libbiomeval/cmake/Modules")

find_package(FFMPEG)
if(FFMPEG_FOUND)
  target_link_libraries( ${NFIQ2_TEST_APP} ${FFMPEG_LIBRARIES})
endif()

find_package(PCSC)
if(PCSC_FOUND)
  target_link_libraries(${NFIQ2_TEST_APP} ${PCSC_LIBRARIES})
endif()

find_package(MPI)
if(MPI_FOUND)
  target_link_libraries(${NFIQ2_TEST_APP} ${MPI_LIBRARIES})
endif()

find_package(HWLOC)
if(HWLOC_FOUND)
  target_link_libraries(${NFIQ2_TEST_APP} ${HWLOC_LIBRARIES})
endif()

find_package(SQLITE3 REQUIRED)
target_link_libraries(${NFIQ2_TEST_APP} ${SQLITE3_LIBRARIES})

find_package(TIFF REQUIRED)
target_link_libraries(${NFIQ2_TEST_APP} ${TIFF_LIBRARIES})

find_package(PNG REQUIRED)
target_link_libraries(${NFIQ2_TEST_APP} ${PNG_LIBRARIES})

find_package(BERKELEYDB REQUIRED)
target_link_libraries(${NFIQ2_TEST_APP} ${BERKELEYDB_LIBRARIES})

if(APPLE)
  find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)  
  target_link_libraries(${NFIQ2_TEST_APP} ${FOUNDATION_FRAMEWORK})
  find_library(SECURITY_FRAMEWORK Security REQUIRED)  
  target_link_libraries(${NFIQ2_TEST_APP} ${SECURITY_FRAMEWORK})
else()
  find_package(OPENSSL REQUIRED)
  target_link_libraries(${NFIQ2_TEST_APP} ${OPENSSL_LIBRARIES})
endif()

find_package(OPENJP2 REQUIRED)
target_link_libraries(${NFIQ2_TEST_APP} ${OPENJP2_LIBRARIES})

find_package(JPEG REQUIRED)
target_link_libraries(${NFIQ2_TEST_APP} ${JPEG_LIBRARIES})

if(MSVC)
target_link_libraries(${NFIQ2_TEST_APP} ${CMAKE_BINARY_DIR}/../../../libbiomeval-prefix/src/libbiomeval-build/lib/$<CONFIGURATION>/${CMAKE_STATIC_LIBRARY_PREFIX}biomeval${CMAKE_STATIC_LIBRARY_SUFFIX})

else()
  target_link_libraries( ${NFIQ2_TEST_APP} ${CMAKE_BINARY_DIR}/../../../libbiomeval-prefix/src/libbiomeval-build/lib/${CMAKE_STATIC_LIBRARY_PREFIX}biomeval${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

set_target_properties(${NFIQ2_TEST_APP}
        PROPERTIES OUTPUT_NAME nfiq2)

install(TARGETS ${NFIQ2_TEST_APP} 
    RUNTIME DESTINATION bin)
install(TARGETS ${PROJECT_NAME} 
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include)
install(FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/../nfiq2rf.yaml"
    DESTINATION bin)

if(UNIX) 
  INSTALL(FILES doc/nfiq2_refresh.1 DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man1")
endif()

if(MSVC)
add_custom_command(TARGET ${NFIQ2_TEST_APP} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_BINARY_DIR}/../../../nfiq2-prefix/src/nfiq2-build/Release ${CMAKE_INSTALL_PREFIX}/bin)
endif()
