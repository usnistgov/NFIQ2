cmake_minimum_required(VERSION 3.28)

project(examples)

option(NFIQ2_EMBEDDED_MODEL "Link against library with embedded model (no NFIQ2::ModelInfo)" NO)

# TODO: Add find_package for NFIQ2
set(NFIQ2_INSTALL_DIR /usr/local/nfiq2 CACHE PATH "Location of NFIQ 2 installation")
include_directories(${NFIQ2_INSTALL_DIR}/include ${NFIQ2_INSTALL_DIR}/nfiq2/include)
################################################################################
set(REQUIRED_LIBS "")

find_library(LIBNFIQ2 nfiq2
	PATHS
		${NFIQ2_INSTALL_DIR}/lib
		${NFIQ2_INSTALL_DIR}/nfiq2/lib
	REQUIRED)
list(APPEND REQUIRED_LIBS ${LIBNFIQ2})


cmake_path(SET OpenCV_DIR NORMALIZE ${LIBNFIQ2}/../cmake/opencv4)
find_package(OpenCV REQUIRED)
list(APPEND REQUIRED_LIBS ${OpenCV_LIBS})

find_library(FJFX FRFXLL_static
	PATHS
		${NFIQ2_INSTALL_DIR}/lib
		${NFIQ2_INSTALL_DIR}/nfiq2/lib
	REQUIRED)
list(APPEND REQUIRED_LIBS ${FJFX})

set(ALL_TARGETS "")
################################################################################

#
# Tool that prints the names of all NFIQ 2 native quality measures.
#

add_executable(print_native_quality_measure_names print_native_quality_measure_names.cpp)
target_link_libraries(print_native_quality_measure_names ${REQUIRED_LIBS})
set_target_properties(print_native_quality_measure_names PROPERTIES
	MACOSX_RPATH YES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO)
list(APPEND ALL_TARGETS print_native_quality_measure_names)

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

if (NFIQ2_EMBEDDED_MODEL)
	target_compile_definitions(example_api PUBLIC NFIQ2_EMBEDDED_MODEL)
endif()

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
