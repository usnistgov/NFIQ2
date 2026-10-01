cmake_minimum_required (VERSION 3.28)

project( nfiq2 )

set (CMAKE_CXX_STANDARD 11)

include(GNUInstallDirs)

# Find external projects (e.g., OpenCV, libbiomeval) in the location where
# CMake has installed them (vs hardcoding the build paths)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX})

get_cmake_property(IS_MULTI_CONFIG GENERATOR_IS_MULTI_CONFIG)

# Configure version header
include("${SUPERBUILD_ROOT_PATH}/NFIQ2/version.cmake")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/nfiq2/version.cpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/nfiq2/version.cpp")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

# FIXME: FingerJet does not properly install headers
include_directories("${SUPERBUILD_ROOT_PATH}/fingerjetfxose/FingerJetFXOSE/libFRFXLL/include")

include_directories("${SUPERBUILD_ROOT_PATH}/digestpp")

option(EMBED_RANDOM_FOREST_PARAMETERS "Embed random forest parameters in library" OFF)
set(EMBEDDED_RANDOM_FOREST_PARAMETER_FCT "0" CACHE STRING
    "ANSI/NIST-ITL 1-2011: Update 2015 friction ridge capture technology (FRCT) code for parameters to embed")

find_package(OpenCV REQUIRED)
set(OpenCV_SHARED ${BUILD_SHARED_LIBS})
include_directories(${OpenCV_INCLUDE_DIRS})

# FIXME: NFIR does not properly install headers
include_directories("${SUPERBUILD_ROOT_PATH}/NFIR/include")

set(SOURCE_FILES
    "src/nfiq2/nfiq2_cbeff.cpp"
    "src/nfiq2/nfiq2_data.cpp"
    "src/nfiq2/nfiq2_fingerprintimagedata.cpp"
    "src/nfiq2/nfiq2_modelinfo.cpp"
    "src/nfiq2/nfiq2_algorithm.cpp"
    "src/nfiq2/nfiq2_algorithm_impl.cpp"
    "src/nfiq2/nfiq2_qualitymeasures.cpp"
    "src/nfiq2/nfiq2_qualitymeasures_impl.cpp"
    "src/nfiq2/nfiq2_timer.cpp"
    "src/nfiq2/nfiq2_exception.cpp"
    "src/nfiq2/version.cpp")

set(FEATURES_FILES
    "src/quality_modules/Module.cpp"
    "src/quality_modules/FDA.cpp"
    "src/quality_modules/FJFXMinutiaeQuality.cpp"
    "src/quality_modules/common_functions.cpp"
    "src/quality_modules/FingerJetFX.cpp"
    "src/quality_modules/ImgProcROI.cpp"
    "src/quality_modules/LCS.cpp"
    "src/quality_modules/Mu.cpp"
    "src/quality_modules/OCLHistogram.cpp"
    "src/quality_modules/OF.cpp"
    "src/quality_modules/QualityMap.cpp"
    "src/quality_modules/RVUPHistogram.cpp")

set(PREDICTION_FILES
    "src/prediction/RandomForestML.cpp")

set(PUBLIC_HEADERS
    "include/nfiq2.hpp"
    "include/nfiq2_data.hpp"
    "include/nfiq2_fingerprintimagedata.hpp"
    "include/nfiq2_constants.hpp"
    "include/nfiq2_modelinfo.hpp"
    "include/nfiq2_algorithm.hpp"
    "include/nfiq2_exception.hpp"
    "include/nfiq2_qualitymeasures.hpp"
    "include/nfiq2_timer.hpp"
    "include/nfiq2_version.hpp")

# FIXME: Some files use M_PI, which under Windows needs special handling
if (MSVC)
	set_source_files_properties(
		"src/quality_modules/common_functions.cpp"
		"src/quality_modules/FDA.cpp"
		"src/quality_modules/OF.cpp"
		"src/quality_modules/QualityMap.cpp"
		PROPERTIES
		COMPILE_DEFINITIONS _USE_MATH_DEFINES)
endif()

set(NFIQ2_LIBRARY_TARGET "nfiq2-lib")
add_library( ${NFIQ2_LIBRARY_TARGET}
  ${SOURCE_FILES}
  ${FEATURES_FILES}
  ${PREDICTION_FILES}
)

set_target_properties(${NFIQ2_LIBRARY_TARGET} PROPERTIES
    PUBLIC_HEADER "${PUBLIC_HEADERS}"
    OUTPUT_NAME "nfiq2"
)

if (EMBED_RANDOM_FOREST_PARAMETERS)
	target_compile_definitions(${NFIQ2_LIBRARY_TARGET} PUBLIC "NFIQ2_EMBED_RANDOM_FOREST_PARAMETERS")
	target_compile_definitions(${NFIQ2_LIBRARY_TARGET} PUBLIC "NFIQ2_EMBEDDED_RANDOM_FOREST_PARAMETERS_FCT=${EMBEDDED_RANDOM_FOREST_PARAMETER_FCT}")
endif()

# FIXME: 1. FingerJet does not install cleantly to expected directory, so we
#           need to make an imported target.
# FIXME: 2. FingerJet always builds both static and shared, so let's just always
#           link since there's no external dependencies in the code.
add_library(fingerjet_lib STATIC IMPORTED)
if (IS_MULTI_CONFIG)
	foreach(cfg ${CMAKE_CONFIGURATION_TYPES})
		find_library(fingerjet_lib_${cfg} FRFXLL_static HINTS
			"${CMAKE_BINARY_DIR}/../../../fingerjetfxose/FingerJetFXOSE/libFRFXLL/src/${cfg}")
		string(TOUPPER ${cfg} CFG_UPPER)
		set_target_properties(fingerjet_lib PROPERTIES IMPORTED_LOCATION_${CFG_UPPER} "${fingerjet_lib_${cfg}}")
	endforeach()
else()
	find_library(fingerjet_lib_path FRFXLL_static HINTS
		"${CMAKE_BINARY_DIR}/../../../fingerjetfxose/FingerJetFXOSE/libFRFXLL/src")
	set_target_properties(fingerjet_lib PROPERTIES IMPORTED_LOCATION "${fingerjet_lib_path}")
endif()

target_link_libraries(${NFIQ2_LIBRARY_TARGET} PUBLIC
	fingerjet_lib
	${OpenCV_LIBS}
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

	if( USE_SANITIZER )
	  target_link_libraries( ${NFIQ2_TEST_APP} "asan" )
	endif()

	# set the required libraries
	add_dependencies(${NFIQ2_TEST_APP} ${NFIQ2_LIBRARY_TARGET})
	set( PROJECT_LIBS ${NFIQ2_LIBRARY_TARGET} )

	if(WIN32)
	  add_definitions("-DNOMINMAX")
	  set( PROJECT_LIBS ${PROJECT_LIBS} ws2_32 )
	elseif(ANDROID)
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

	find_library(LIBNFIR nfir PATHS
	    ${CMAKE_BINARY_DIR}/../../../nfir-prefix/src/nfir-build/
	    ${CMAKE_BINARY_DIR}/../../../nfir-prefix/src/nfir-build/$<CONFIGURATION>)
	target_link_libraries(${NFIQ2_TEST_APP} ${LIBNFIR})

	find_package(biomeval REQUIRED)
	target_link_libraries(${NFIQ2_TEST_APP} biomeval::biomeval)

	# Getopt for Windows
	if(MSVC)
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

if(WIN32)
	# copy runtime DLLs next to the exe
	install(TARGETS ${NFIQ2_TEST_APP}
		RUNTIME_DEPENDENCY_SET app_deps
	)
	install(RUNTIME_DEPENDENCY_SET app_deps
		PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
		POST_EXCLUDE_REGEXES ".*system32.*"
		DIRECTORIES
			$<TARGET_FILE_DIR:${NFIQ2_TEST_APP}>
			# FIXME: FingerJet required link_directories() which doesn't propagate here
			"${CMAKE_BINARY_DIR}/../../../fingerjetfxose/FingerJetFXOSE/libFRFXLL/src"
			"${CMAKE_BINARY_DIR}/../../../fingerjetfxose/FingerJetFXOSE/libFRFXLL/src/$<$<BOOL:${IS_MULTI_CONFIG}>:$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>>"
)
elseif(APPLE)
	# bundle dylibs
	set_target_properties(${NFIQ2_TEST_APP} PROPERTIES
	    INSTALL_RPATH "@executable_path/../lib"
	    BUILD_WITH_INSTALL_RPATH FALSE)

	install(TARGETS ${NFIQ2_TEST_APP}
	    RUNTIME_DEPENDENCY_SET deps
	    RUNTIME DESTINATION bin)

	install(RUNTIME_DEPENDENCY_SET deps
	    DESTINATION lib
	    PRE_EXCLUDE_REGEXES "^/usr/lib" "^/System")

	# fix install names at install time
	set(_app_filename "$<TARGET_FILE_NAME:${NFIQ2_TEST_APP}>")
	install(CODE "set(_app_filename \"${_app_filename}\")")
	install(CODE [[
	  file(GLOB _dylibs "${CMAKE_INSTALL_PREFIX}/lib/*.dylib")
	  foreach(_lib IN LISTS _dylibs)
	    # give the copied lib an @rpath id
	    get_filename_component(_name "${_lib}" NAME)
	    execute_process(COMMAND install_name_tool -id "@rpath/${_name}" "${_lib}")
	  endforeach()

	  # rewrite MacPorts/Homrebew references in the app and every copied dylib
	  foreach(_bin ${_dylibs} "${CMAKE_INSTALL_PREFIX}/bin/${_app_filename}")
	    execute_process(COMMAND otool -L "${_bin}" OUTPUT_VARIABLE _out)
	    string(REGEX MATCHALL "(/opt/local|/opt/homebrew|/usr/local)/[^ ]+\\.dylib" _refs "${_out}")
	    foreach(_ref IN LISTS _refs)
	      get_filename_component(_refname "${_ref}" NAME)
	      execute_process(COMMAND install_name_tool -change
		"${_ref}" "@rpath/${_refname}" "${_bin}")
	    endforeach()
	  endforeach()
	]])
endif()

endif(BUILD_NFIQ2_CLI)

install(TARGETS ${NFIQ2_LIBRARY_TARGET}
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT install_staging)

# FIXME: FingerJet doesn't have an install target
install(FILES
	${CMAKE_BINARY_DIR}/../../../fingerjetfxose/FingerJetFXOSE/libFRFXLL/src/$<$<BOOL:${IS_MULTI_CONFIG}>:$<$<CONFIG:Debug>:Debug/>$<$<CONFIG:Release>:Release/>>${CMAKE_STATIC_LIBRARY_PREFIX}FRFXLL_static${CMAKE_STATIC_LIBRARY_SUFFIX}
	${CMAKE_BINARY_DIR}/../../../fingerjetfxose/FingerJetFXOSE/libFRFXLL/src/$<$<BOOL:${IS_MULTI_CONFIG}>:$<$<CONFIG:Debug>:Debug/>$<$<CONFIG:Release>:Release/>>${CMAKE_SHARED_LIBRARY_PREFIX}FRFXLL${CMAKE_SHARED_LIBRARY_SUFFIX}
	DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	COMPONENT install_staging)

# Include VC++ redistributable if needed
if (WIN32 AND "${CMAKE_MSVC_RUNTIME_LIBRARY}" MATCHES "DLL")
	set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
	include(InstallRequiredSystemLibraries)
endif()
