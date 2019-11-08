project( Nfiq2Api ) 

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

find_package(OpenCV REQUIRED NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH HINTS ${CMAKE_BINARY_DIR})
set(OpenCV_SHARED ON)
include_directories(${OpenCV_INCLUDE})

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

target_link_libraries( "${PROJECT_NAME}" ${PROJECT_LIBS} ${OpenCV_LIBS} ) 
