project( fingerjetfxose ) 

# download sources
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/download/)
  set( FINGERJET_SRC "https://github.com/FingerJetFXOSE/FingerJetFXOSE/archive/master.zip")
  set( FINGERJET_DST "${CMAKE_CURRENT_SOURCE_DIR}/download/tmp.zip")
  message("Download ${FINGERJET_SRC} to ${FINGERJET_DST}")
  file(DOWNLOAD "${FINGERJET_SRC}" "${FINGERJET_DST}")
  execute_process( COMMAND cmake -E tar xfz tmp.zip WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/download/ )
  file(REMOVE "${FINGERJET_DST}")
  if( WIN32 )
    # using the msys2 windows port for patch (requires 64 bit windows)
    execute_process( COMMAND patch -s -p0 --verbose -i nfiq2.patch -d ${CMAKE_CURRENT_SOURCE_DIR} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/windows )
  else()
    execute_process( COMMAND patch -s -p0 --verbose -i nfiq2.patch -d ${CMAKE_CURRENT_SOURCE_DIR} )
  endif()
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_definitions( "-O3 -std=c++11 -fvisibility=hidden -Wno-unknown-pragmas -Wno-unused-but-set-variable")
  if(NOT "${TARGET_PLATFORM}" MATCHES "win*")
    add_definitions( "-fPIC -fvisibility=hidden")
  endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  add_definitions( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4005 /wd4996 /nologo /MT" )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_definitions( "-O3 -fPIC -fvisibility=hidden" )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  add_definitions( "-O3 -fPIC -fvisibility=hidden" )
endif()

set(FINGERJET_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/download/FingerJetFXOSE-master")

# add include directories
include_directories("${FINGERJET_ROOT}")
include_directories("${FINGERJET_ROOT}/src/libFRFXLL/include")
include_directories("${FINGERJET_ROOT}/src/libFRFXLL/src/algorithm")

set(SOURCE_FILES "")
file( GLOB SOURCE_FILES "${FINGERJET_ROOT}/src/libFRFXLL/src/libFRFXLL/*.cpp" ) 
add_library( "frfxll" STATIC
  ${SOURCE_FILES}
)

if(EXISTS "${FINGERJET_ROOT}/src/libFRFXLL/src/algorithm/version.h")
  FILE(READ "${FINGERJET_ROOT}/src/libFRFXLL/src/algorithm/version.h" VERSION_H_CONTENT)

  if("${VERSION_H_CONTENT}" MATCHES "#define[ ]+FRFXLL_MAJOR[ ]+\"?([0-9]+)\"?[ ]*\r?\n")
    set( FRFXLL_MAJOR "${CMAKE_MATCH_1}" )
  endif()

  if("${VERSION_H_CONTENT}" MATCHES "#define[ ]+FRFXLL_MINOR[ ]+\"?([0-9]+)\"?[ ]*\r?\n")
    set( FRFXLL_MINOR "${CMAKE_MATCH_1}" )
  endif()

  if("${VERSION_H_CONTENT}" MATCHES "#define[ ]+FRFXLL_REVISION[ ]+\"?([0-9]+)\"?[ ]*\r?\n")
    set( FRFXLL_REVISION "${CMAKE_MATCH_1}" )
  endif()

  set( FRFXLL_VERSION "${FRFXLL_MAJOR}.${FRFXLL_MINOR}.${FRFXLL_REVISION}" PARENT_SCOPE)
endif()
