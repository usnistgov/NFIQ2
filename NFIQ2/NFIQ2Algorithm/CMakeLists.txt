cmake_minimum_required (VERSION 3.3)

project( nfiq2 )

set (CMAKE_CXX_STANDARD 11)

set(NFIQ2_CONTAINER_DIR nfiq2)
include(GNUInstallDirs)
string(APPEND CMAKE_INSTALL_LIBDIR "/${NFIQ2_CONTAINER_DIR}")
string(APPEND CMAKE_INSTALL_INCLUDEDIR "/${NFIQ2_CONTAINER_DIR}")
string(APPEND CMAKE_INSTALL_DATADIR "/${NFIQ2_CONTAINER_DIR}")

include( "${SUPERBUILD_ROOT_PATH}/cmake/colors.cmake" )
include( "${SUPERBUILD_ROOT_PATH}/cmake/target.cmake" )
include( "${SUPERBUILD_ROOT_PATH}/cmake/compiler.cmake" )

# Configure version header
include("${SUPERBUILD_ROOT_PATH}/NFIQ2/version.cmake")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/nfiq2/version.cpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/nfiq2/version.cpp")

# add include directories
include_directories("${CMAKE_BINARY_DIR}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

# add libbiomeval include directories
include_directories("${SUPERBUILD_ROOT_PATH}/libbiomeval/src/include")

include_directories("${SUPERBUILD_ROOT_PATH}/fingerjetfxose/FingerJetFXOSE/libFRFXLL/include")

include_directories("${SUPERBUILD_ROOT_PATH}/digestpp")

set( OpenCV_DIR ${CMAKE_BINARY_DIR}/../../../OpenCV-prefix/src/OpenCV-build)
find_package(OpenCV REQUIRED NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH HINTS ${OpenCV_DIR})
set(OpenCV_SHARED ON)
include_directories(${OpenCV_INCLUDE_DIRS})

set(SOURCE_FILES "")
file( GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/nfiq2/*.cpp" )
file( GLOB FEATURES_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/features/*.cpp" )
file( GLOB PREDICTION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/prediction/*.cpp" )

set(PUBLIC_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/nfiq2/nfiq2.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/nfiq2/interfacedefinitions.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/nfiq2/fingerprintimagedata.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/nfiq2/data.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/nfiq2/nfiqexception.hpp"
)

set(NFIQ2_STATIC_LIBRARY_TARGET "nfiq2-static-lib")
add_library( ${NFIQ2_STATIC_LIBRARY_TARGET} STATIC
  ${SOURCE_FILES}
  ${FEATURES_FILES}
  ${PREDICTION_FILES}
)

set_target_properties(${NFIQ2_STATIC_LIBRARY_TARGET} PROPERTIES
    PUBLIC_HEADER "${PUBLIC_HEADERS}"
    OUTPUT_NAME "nfiq2"
)

# FIXME: Change to "${CMAKE_INSTALL_PREFIX}/lib" once FJFX builds
# FIXME: are updated.
link_directories("${CMAKE_BINARY_DIR}/../../../fingerjetfxose/FingerJetFXOSE/libFRFXLL/src")
link_directories("${CMAKE_BINARY_DIR}/../../../fingerjetfxose/FingerJetFXOSE/libFRFXLL/src/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>")
target_link_libraries(${NFIQ2_STATIC_LIBRARY_TARGET} PUBLIC
	FRFXLL_static
	${OpenCV_LIBS}
)

if(USE_SANITIZER)
	target_link_libraries(${NFIQ2_STATIC_LIBRARY_TARGET} "asan")
endif()

# New NFIQ2 TOOL
if (BUILD_NFIQ2_CLI)
	set( NFIQ2_TEST_APP "nfiq2-bin" )

	add_executable(${NFIQ2_TEST_APP}
	  "${CMAKE_CURRENT_SOURCE_DIR}/src/tool/nfiq2_ui_refresh.cpp"
	  "${CMAKE_CURRENT_SOURCE_DIR}/src/tool/nfiq2_ui_log.cpp"
	  "${CMAKE_CURRENT_SOURCE_DIR}/src/tool/nfiq2_ui_utils.cpp"
	  "${CMAKE_CURRENT_SOURCE_DIR}/src/tool/nfiq2_ui_exception.cpp"
	  "${CMAKE_CURRENT_SOURCE_DIR}/src/tool/nfiq2_ui_threadedlog.cpp"
	  "${CMAKE_CURRENT_SOURCE_DIR}/src/tool/nfiq2_ui_image.cpp"
	  "${CMAKE_CURRENT_SOURCE_DIR}/src/tool/nfiq2_ui_types.cpp"
	)

	if( USE_SANITIZER )
	  target_link_libraries( ${NFIQ2_TEST_APP} "asan" )
	endif()

	# set the required libraries
	add_dependencies(${NFIQ2_TEST_APP} ${NFIQ2_STATIC_LIBRARY_TARGET})
	set( PROJECT_LIBS ${NFIQ2_STATIC_LIBRARY_TARGET} )

	if("${TARGET_PLATFORM}" MATCHES "win*")
	  add_definitions("-DNOMINMAX")
	  set( PROJECT_LIBS ${PROJECT_LIBS} ws2_32 )
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

	target_link_libraries( ${NFIQ2_TEST_APP} ${PROJECT_LIBS} )

	# Windows builds require crypt32 supplemental library for OpenSSL
	if(MSVC)
	  target_link_libraries(${NFIQ2_TEST_APP} "crypt32")
	endif()

	# Linking in biomeval lib - must occur before individual libs due to linker bug
	if(MSVC)
	  target_link_libraries(${NFIQ2_TEST_APP} ${CMAKE_BINARY_DIR}/../../../libbiomeval-prefix/src/libbiomeval-build/lib/$<CONFIGURATION>/${CMAKE_STATIC_LIBRARY_PREFIX}biomeval${CMAKE_STATIC_LIBRARY_SUFFIX})
	else()
	  target_link_libraries( ${NFIQ2_TEST_APP} ${CMAKE_BINARY_DIR}/../../../libbiomeval-prefix/src/libbiomeval-build/lib/${CMAKE_STATIC_LIBRARY_PREFIX}biomeval${CMAKE_STATIC_LIBRARY_SUFFIX})
	endif()

	# The following is lifted from libbiomeval and can be removed once find_package has been implemented for the library.
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
	  find_package(OpenSSL REQUIRED)
	  target_link_libraries(${NFIQ2_TEST_APP} ${OPENSSL_LIBRARIES})
	endif()

	find_package(OPENJP2 REQUIRED)
	target_link_libraries(${NFIQ2_TEST_APP} ${OPENJP2_LIBRARIES})

	find_package(JPEG REQUIRED)
	target_link_libraries(${NFIQ2_TEST_APP} ${JPEG_LIBRARIES})

	# Getopt for Windows
	if(MSVC)
	  if(CMAKE_SIZEOF_VOID_P EQUAL 4)
	    list(APPEND CMAKE_PREFIX_PATH ${_VCPKG_INSTALLED_DIR}/x86-windows/)
	  elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
	    list(APPEND CMAKE_PREFIX_PATH ${_VCPKG_INSTALLED_DIR}/x64-windows/)
	  endif()

	  find_library(GETOPT_LIB getopt REQUIRED)
	  find_file(GETOPT_HEADER getopt.h REQUIRED)
	  get_filename_component(GETOPT_HEADER_DIR "${GETOPT_HEADER}" DIRECTORY)
	  target_include_directories(${NFIQ2_TEST_APP} PRIVATE "${GETOPT_HEADER_DIR}")
	  target_link_libraries(${NFIQ2_TEST_APP} "${GETOPT_LIB}")
	endif()

	set_target_properties(${NFIQ2_TEST_APP}
	  PROPERTIES RUNTIME_OUTPUT_NAME nfiq2)

	install(TARGETS ${NFIQ2_TEST_APP}
	    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	    COMPONENT install_staging)

	if (UNIX)
		install(FILES
		    "${CMAKE_CURRENT_SOURCE_DIR}/../nist_plain_tir-ink.txt"
		    "${CMAKE_CURRENT_SOURCE_DIR}/../nist_plain_tir-ink.yaml"
		    DESTINATION ${CMAKE_INSTALL_DATADIR}
		    COMPONENT install_staging)
	else()
		install(FILES
		    "${CMAKE_CURRENT_SOURCE_DIR}/../nist_plain_tir-ink.txt"
		    "${CMAKE_CURRENT_SOURCE_DIR}/../nist_plain_tir-ink.yaml"
		    DESTINATION ${CMAKE_INSTALL_BINDIR}
		    COMPONENT install_staging)
	endif()

	# Install Man Page for Unix Systems
	if(UNIX)
	  INSTALL(FILES doc/nfiq2_tool.1 DESTINATION "${CMAKE_INSTALL_MANDIR}/man1" RENAME nfiq2.1
	  COMPONENT install_staging)
	endif()

	# Copies getopt dll for Windows builds
	if(MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 4)
		install(FILES
		    ${_VCPKG_INSTALLED_DIR}/x86-windows/bin/getopt.dll
		    DESTINATION ${CMAKE_INSTALL_BINDIR}
		    COMPONENT install_staging)
	endif()

	if(MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 8)
		install(FILES
		    ${_VCPKG_INSTALLED_DIR}/x64-windows/bin/getopt.dll
		    DESTINATION ${CMAKE_INSTALL_BINDIR}
		    COMPONENT install_staging)
	endif()
endif(BUILD_NFIQ2_CLI)

install(TARGETS ${NFIQ2_STATIC_LIBRARY_TARGET}
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT install_staging)

# FIXME: FingerJet doesn't have an install target
install(FILES
	${CMAKE_BINARY_DIR}/../../../fingerjetfxose/FingerJetFXOSE/libFRFXLL/src/$<$<CONFIG:Debug>:Debug/>$<$<CONFIG:Release>:Release/>${CMAKE_STATIC_LIBRARY_PREFIX}FRFXLL_static${CMAKE_STATIC_LIBRARY_SUFFIX}
	${CMAKE_BINARY_DIR}/../../../fingerjetfxose/FingerJetFXOSE/libFRFXLL/src/$<$<CONFIG:Debug>:Debug/>$<$<CONFIG:Release>:Release/>${CMAKE_SHARED_LIBRARY_PREFIX}FRFXLL${CMAKE_SHARED_LIBRARY_SUFFIX}
	DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	COMPONENT install_staging)
