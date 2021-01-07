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
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/nfiq2_version.cpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/nfiq2_version.cpp")

# add include directories
include_directories("${CMAKE_BINARY_DIR}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/features")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/prediction")

# add NFIQ2 tool include directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/tool")

# add libbiomeval include directories
include_directories("${SUPERBUILD_ROOT_PATH}/libbiomeval/src/include")

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
link_directories( "${CMAKE_BINARY_DIR}/../../../fingerjetfxose/FingerJetFXOSE/libFRFXLL/src" )
link_directories( "${CMAKE_BINARY_DIR}/../../../biomdi" )

set(SOURCE_FILES "")
file( GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" )
list( REMOVE_ITEM SOURCE_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/NFIQ2.cpp"
)

file( GLOB FEATURES_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/features/*.cpp" )
file( GLOB PREDICTION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/prediction/*.cpp" )

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
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    PUBLIC_HEADER "${PUBLIC_HEADERS}"
)

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
	# FIXME: This should be DATAROOTDIR
	install(FILES
	    "${CMAKE_CURRENT_SOURCE_DIR}/../nfiq2rf.yaml"
	    DESTINATION ${CMAKE_INSTALL_BINDIR}
	    COMPONENT install_staging)

	add_custom_command(TARGET ${NFIQ2_TEST_APP} POST_BUILD
	    COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_CURRENT_SOURCE_DIR}/../nfiq2rf.yaml ${SUPERBUILD_ROOT_PATH}/complianceTestSet/nfiq2rf.yaml)

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

install(TARGETS ${PROJECT_NAME}
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

# FIXME: biomdi is going away and doesn't have an install target
install(FILES
	${CMAKE_BINARY_DIR}/../../../biomdi/$<$<CONFIG:Debug>:Debug/>$<$<CONFIG:Release>:Release/>${CMAKE_STATIC_LIBRARY_PREFIX}biomdi${CMAKE_STATIC_LIBRARY_SUFFIX}
	${CMAKE_BINARY_DIR}/../../../biomdi/$<$<CONFIG:Debug>:Debug/>$<$<CONFIG:Release>:Release/>${CMAKE_STATIC_LIBRARY_PREFIX}fir${CMAKE_STATIC_LIBRARY_SUFFIX}
	${CMAKE_BINARY_DIR}/../../../biomdi/$<$<CONFIG:Debug>:Debug/>$<$<CONFIG:Release>:Release/>${CMAKE_STATIC_LIBRARY_PREFIX}fmr${CMAKE_STATIC_LIBRARY_SUFFIX}
	${CMAKE_BINARY_DIR}/../../../biomdi/$<$<CONFIG:Debug>:Debug/>$<$<CONFIG:Release>:Release/>${CMAKE_STATIC_LIBRARY_PREFIX}frf${CMAKE_STATIC_LIBRARY_SUFFIX}
	${CMAKE_BINARY_DIR}/../../../biomdi/$<$<CONFIG:Debug>:Debug/>$<$<CONFIG:Release>:Release/>${CMAKE_STATIC_LIBRARY_PREFIX}iid${CMAKE_STATIC_LIBRARY_SUFFIX}
	DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	COMPONENT install_staging)

