#
# NFIQ2-API CmakeLists_NFIQ2-API...
#

#
# The CMake program makes it possible to write simple configuration files to control a project’s
# compilation process and generate native build files across platforms.
#

#
# Sets the minimum required version of cmake for a project.
# Call the cmake_minimum_required() command at the beginning of the top-level CMakeLists.txt file even before calling the project() command.
# It is important to establish version and policy settings before invoking other commands whose behavior they may affect.
#
# CMake Policies -> https://cmake.org/cmake/help/v3.3/manual/cmake-policies.7.html
#
cmake_minimum_required (VERSION 3.3)

#
# Sets the name of the project, and stores it in the variable PROJECT_NAME.
# When called from the top-level CMakeLists.txt also stores the project name in the variable CMAKE_PROJECT_NAME.
#
project( Nfiq2Api )

set(CMAKE_CXX_STANDARD 11)

# add files from folder
set(SOURCE_FILES "nfiq2api.cpp")
set(HEADER_FILES "nfiq2api.h")

include( "${SUPERBUILD_ROOT_PATH}/cmake/colors.cmake" )
include( "${SUPERBUILD_ROOT_PATH}/cmake/target.cmake" )
include( "${SUPERBUILD_ROOT_PATH}/cmake/compiler.cmake" )

#
# Add include directories
#
include_directories("${CMAKE_BINARY_DIR}")
include_directories("${SUPERBUILD_ROOT_PATH}/NFIQ2/NFIQ2Algorithm")
include_directories("${SUPERBUILD_ROOT_PATH}/NFIQ2/NFIQ2Algorithm/include")
include_directories("${SUPERBUILD_ROOT_PATH}/fingerjetfxose/download/FingerJetFXOSE-master/src/libFRFXLL/include")



#######################################################################################################################################

######################
### OpenCV Support ###
######################

#
# OpenCV Estático.
#
# Shared for OpenCV Support...
#
# set( OpenCV_DIR ${CMAKE_BINARY_DIR}/../../../OpenCV-prefix/src/OpenCV-build)
# find_package(OpenCV REQUIRED NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH HINTS ${OpenCV_DIR})
# set(OpenCV_SHARED ON)
# include_directories(${OpenCV_INCLUDE})
#

#
# Static for OpenCV Support...
#
set(OpenCV_DIR "C:/opencv_static_4_7_0/x64-Release/x64/vc17/staticlib")
find_package(OpenCV REQUIRED core imgproc imgcodecs videoio ml highgui)
set(OpenCV_STATIC ON)
include_directories(${OpenCV_INCLUDE_DIRS})

#
# Messages INFO...
#
message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

#######################################################################################################################################

# set and increment version
include( "${SUPERBUILD_ROOT_PATH}/NFIQ2/version.cmake" )

include(GNUInstallDirs)
link_directories( "${CMAKE_BINARY_DIR}/../../../nfiq2-prefix/src/nfiq2-build" )
link_directories( "${CMAKE_BINARY_DIR}/../../../fingerjetfxose/FingerJetFXOSE/libFRFXLL/src" )

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

set_target_properties(${PROJECT_NAME} PROPERTIES
    PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/nfiq2api.h"
)

target_compile_definitions("${PROJECT_NAME}"
  PRIVATE I_UNDERSTAND_THIS_NFIQ2_API_WILL_BE_REMOVED)

string( REPLACE "." "" OPENCV_VERSION2 "${OPENCV_VERSION}")

if("${TARGET_PLATFORM}" MATCHES "win*")
  # remove the lib prefix for Windows
  set_target_properties( ${PROJECT_NAME} PROPERTIES PREFIX "" )
endif()

# add required link libraries
set( PROJECT_LIBS
  nfiq2
  FRFXLL_static
)

if( USE_SANITIZER )
  target_link_libraries( ${PROJECT_NAME} "asan" )
endif()

if("${TARGET_PLATFORM}" MATCHES "win*")
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
  )
else()
  find_package(Threads REQUIRED)
  set( PROJECT_LIBS ${PROJECT_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_DL_LIBS}
  )
endif()

target_link_libraries( "${PROJECT_NAME}" ${PROJECT_LIBS} ${OpenCV_LIBS} )

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT install_staging)
