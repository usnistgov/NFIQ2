cmake_minimum_required (VERSION 3.28)

project( Nfiq2Api )

set(CMAKE_CXX_STANDARD 11)

# add files from folder
set(SOURCE_FILES "nfiq2api.cpp")
set(HEADER_FILES "nfiq2api.h")

# add include directories
include_directories("${CMAKE_BINARY_DIR}")

include_directories("${SUPERBUILD_ROOT_PATH}/NFIQ2/NFIQ2Algorithm")
include_directories("${SUPERBUILD_ROOT_PATH}/NFIQ2/NFIQ2Algorithm/include")

include_directories("${SUPERBUILD_ROOT_PATH}/fingerjetfxose/download/FingerJetFXOSE-master/src/libFRFXLL/include")

find_package(OpenCV REQUIRED)
set(OpenCV_SHARED ${BUILD_SHARED_LIBS})
include_directories(${OpenCV_INCLUDE_DIRS})
# set and increment version
include( "${SUPERBUILD_ROOT_PATH}/NFIQ2/version.cmake" )

include(GNUInstallDirs)
link_directories( "${CMAKE_BINARY_DIR}/../../../nfiq2-prefix/src/nfiq2-build" )
link_directories( "${CMAKE_BINARY_DIR}/../../../fingerjetfxose/FingerJetFXOSE/libFRFXLL/src" )

# add targets
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_library( ${PROJECT_NAME}
      "nfiq2api.def"
      ${SOURCE_FILES}
      ${VERSION_FILES}
    )
else()
    add_library( ${PROJECT_NAME}
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

if(WIN32)
  # remove the lib prefix for Windows
  set_target_properties( ${PROJECT_NAME} PROPERTIES PREFIX "" )
endif()

# add required link libraries
set( PROJECT_LIBS
  nfiq2
  FRFXLL_static
)

if (EMBED_RANDOM_FOREST_PARAMETERS)
	target_compile_definitions(${PROJECT_NAME} PUBLIC "NFIQ2_EMBED_RANDOM_FOREST_PARAMETERS")
	target_compile_definitions(${PROJECT_NAME} PUBLIC "NFIQ2_EMBEDDED_RANDOM_FOREST_PARAMETERS_FCT=${EMBEDDED_RANDOM_FOREST_PARAMETER_FCT}")
endif()

if(WIN32)
    set( PROJECT_LIBS ${PROJECT_LIBS}
      ws2_32
    )
elseif(ANDROID)
  find_package(Threads REQUIRED)
  set( PROJECT_LIBS ${PROJECT_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
    android
    log
  )
else()
  find_package(Threads REQUIRED)
  set( PROJECT_LIBS ${PROJECT_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_DL_LIBS}
  )
endif()

target_link_libraries( "${PROJECT_NAME}" PUBLIC ${PROJECT_LIBS} ${OpenCV_LIBS} )

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT install_staging)
