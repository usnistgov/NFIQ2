#
# NFIQ2 Example...
#

#
#	The CMake program makes it possible to write simple configuration files to control a project’s
#	compilation process and generate native build files across platforms.
#

#
# Sets the minimum required version of cmake for a project.
# Call the cmake_minimum_required() command at the beginning of the top-level CMakeLists.txt file even before calling the project() command.
# It is important to establish version and policy settings before invoking other commands whose behavior they may affect.
#
# CMake Policies -> https://cmake.org/cmake/help/v3.3/manual/cmake-policies.7.html
#
cmake_minimum_required(VERSION 3.11)

#
# Sets the name of the project, and stores it in the variable PROJECT_NAME.
# When called from the top-level CMakeLists.txt also stores the project name in the variable CMAKE_PROJECT_NAME.
#
project(examples)

#
# Record the specified message text in the log.
# If more than one message string is given, they are concatenated into a single message with no separator between the strings.
#
message(STATUS "NFIQ2 RUNNING EXAMPLE...")

#
# LIBNFIQ2 Static Support...
#
option(LINK_AGAINST_STATIC_LIBNFIQ2 "Link against static (vs shared) libnfiq2" YES)

#
# TODO: Add find_package for NFIQ2
#
set(NFIQ2_INSTALL_DIR C:/Users/marco.silva/NFIQ2/build/install_staging/nfiq2 CACHE STRING "Location of NFIQ 2 installation")
include_directories(${NFIQ2_INSTALL_DIR})
#

################################################################################

set(REQUIRED_LIBS "")

if (LINK_AGAINST_STATIC_LIBNFIQ2)
	find_library(LIBNFIQ2_STATIC
		${CMAKE_STATIC_LIBRARY_PREFIX}nfiq2${CMAKE_STATIC_LIBRARY_SUFFIX}
		PATHS
			${NFIQ2_INSTALL_DIR}/lib
		REQUIRED)
	find_library(LIBFRFXLL_STATIC
		${CMAKE_STATIC_LIBRARY_PREFIX}FRFXLL_static${CMAKE_STATIC_LIBRARY_SUFFIX}
		PATHS
			${NFIQ2_INSTALL_DIR}/lib
		REQUIRED)

	set(OpenCV_DIR "C:/opencv_static_4_7_0/x64-Release/x64/vc17/staticlib")
	find_package(OpenCV REQUIRED core imgproc imgcodecs videoio ml highgui)
	set(OpenCV_STATIC ON)
	include_directories(${OpenCV_INCLUDE_DIRS})
		#HINTS
			#${NFIQ2_INSTALL_DIR}/lib/cmake/opencv4
			#${NFIQ2_INSTALL_DIR})

	list(APPEND REQUIRED_LIBS
		${LIBNFIQ2_STATIC}
		${LIBFRFXLL_STATIC}
		${OpenCV_LIBS})

	# Static Windows runtime
	if (MSVC)
		foreach(flag_var
		    CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
		    CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
			if(${flag_var} MATCHES "/MD")
				string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
			endif()
			if(${flag_var} MATCHES "/MDd")
				string(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
			endif()
		endforeach(flag_var)
	endif()
else()
	find_library(LIBNFIQ2_SHARED
		nfiq2
		PATHS
			${NFIQ2_INSTALL_DIR}/lib
			${NFIQ2_INSTALL_DIR}/nfiq2/lib
		REQUIRED)

	list(APPEND REQUIRED_LIBS
		${LIBNFIQ2_SHARED})
endif()

set(ALL_TARGETS "")
################################################################################

#
# Tool that prints the names of all NFIQ 2 features.
#

add_executable(print_feature_names print_feature_names.cpp)
target_link_libraries(print_feature_names ${REQUIRED_LIBS})
set_target_properties(print_feature_names PROPERTIES
	MACOSX_RPATH YES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO)
list(APPEND ALL_TARGETS print_feature_names)

#
# Tool that computes quality scores
#

add_executable(example_api example_api.cpp)
target_link_libraries(example_api ${REQUIRED_LIBS})
target_include_directories(example_api PRIVATE ${OpenCV_INCLUDE_DIRS})
set_target_properties(example_api PROPERTIES
	MACOSX_RPATH YES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO)
list(APPEND ALL_TARGETS example_api)

################################################################################

# Enable warnings for all targets
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(WARNING_FLAGS -Wall -Wextra -Wconversion -Wsign-conversion -pedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(WARNING_FLAGS /W4)
endif()
if (WARNING_FLAGS)
	set_target_properties(${ALL_TARGETS} PROPERTIES
		COMPILE_OPTIONS "${WARNING_FLAGS}")
endif()

################################################################################
