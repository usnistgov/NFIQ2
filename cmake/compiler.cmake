option(USE_SANITIZER "Integrate address sanitizer" OFF)

option(BUILD_STATIC "Build static library" ON)
option(BUILD_SHARED "Build shared library" OFF)

include(CheckCXXSourceRuns)
function(checkForStaticLibs)
	if (APPLE)
		return()
	endif()

	# Only check GNU (we know MSVC will work)
	if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
		if (WIN32)
			set(CMAKE_REQUIRED_FLAGS "-static;-static-libgcc;-static-libstdc++")
			set(CMAKE_REQUIRED_LINK_OPTIONS "-static;-static-libgcc;-static-libstdc++;-add-stdcall-alias;-enable-stdcall-fixup")
		elseif(UNIX)
			set(CMAKE_REQUIRED_FLAGS "-static-libgcc;-static-libstdc++")
			set(CMAKE_REQUIRED_LINK_OPTIONS "-z defs;-static-libgcc;-static-libstdc++")
		endif()

		check_cxx_source_runs("int main() { return 0; }" STATIC_SYSTEM_LIBS)
		if (NOT STATIC_SYSTEM_LIBS)
			message(WARNING "You are missing one or both of static-libgcc and static-libstdc++")
		endif()
	endif()
endfunction()

if (BUILD_STATIC)
	checkForStaticLibs()
endif()

# Pre-processor definitions
set(NFIQ2_DEFINITIONS)
# C++ flags
set(NFIQ2_CXX_FLAGS)
# Library linker flags
set(NFIQ2_LIBRARY_LINKER_FLAGS)
# CLI linker flags
set(NFIQ2_CLI_LINKER_FLAGS)

# Enable definitions of mathematical constants
list(APPEND NFIQ2_DEFINITIONS "_USE_MATH_DEFINES")

# Don't use min/max macros from Windows header
if (WIN32)
	list(APPEND NFIQ2_DEFINITIONS "NOMINMAX")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
	message(STATUS "Detected ${CMAKE_CXX_COMPILER_ID} compiler")

	if (USE_SANITIZER)
		list(APPEND NFIQ2_CXX_FLAGS -fsanitize=address -fsanitize=leak -fstack-check -fno-omit-frame-pointer)
		if (BUILD_STATIC)
			list(APPEND NFIQ2_CXX_FLAGS -static-libasan)
		endif()
	endif()

	# Position-independent library code
	list(APPEND NFIQ2_CXX_FLAGS -fPIC)

	# Linking static runtimes
	if (BUILD_STATIC)
		list(APPEND NFIQ2_CXX_FLAGS -static)
		if (CMAKE_CXX_COMPILER_ID EQUAL "GNU")
			list(APPEND NFIQ2_CXX_FLAGS -static-libgcc -static-libstdc++)

			list(APPEND NFIQ2_CXX_FLAGS "-static" "-static-libgcc" "-static-libstdc++")
			if (WIN32)
				list(APPEND NFIQ2_LIBRARY_LINKER_FLAGS "-static" "-static-libgcc" "-static-libstdc++" "-add-stdcall-alias" "-enable-stdcall-fixup")
			elseif(UNIX)
				list(APPEND NFIQ2_CXX_FLAGS "-static-libgcc" "-static-libstdc++")
				list(APPEND NFIQ2_LIBRARY_LINKER_FLAGS "-static-libgcc" "static-libstdc++")
			endif()
		endif()
	endif()

	# Disallow undefined static symbols (failsafe)
	if (CMAKE_CXX_COMPILER_ID EQUAL GNU)
		list(APPEND NFIQ2_CLI_LINKER_FLAGS "-z defs")
		list(APPEND NFIQ2_LIBRARY_LINKER_FLAGS "-z defs")
	endif()

	if(32BITS)
		list(APPEND NFIQ2_CXX_FLAGS -m32)
	elseif(64BITS)
		list(APPEND NFIQ2_CXX_FLAGS -m64)
	endif()

	# Compiler warnings
	list(APPEND NFIQ2_CXX_FLAGS -Wall -Wpedantic -Wextra
#	    -Wconversion
#	    -Wsign-conversion -Wimplicit-fallthrough -Wdouble-promotion -Wshadow
#	    -Wnon-virtual-dtor -Woverloaded-virtual -Wunused -Wformat=2
	)

	# clang vs. gnu specific warnings
	if (CMAKE_CXX_COMPILER_ID EQUAL GNU)
		list(APPEND NFIQ2_CXX_FLAGS
		)
	else()
		list(APPEND NFIQ2_CXX_FLAGS
#		    -Wdocumentation
		)
	endif()

elseif (MSVC)
	# No copyright notice
	list(APPEND NFIQ2_CXX_FLAGS /nologo)

	# No warnings for unsecure functions
	list(APPEND NFIQ2_DEFINITIONS _CRT_SECURE_NO_WARNINGS)

	# Warnings
	list(APPEND NFIQ2_CXX_FLAGS /W3)
else()
	message(FATAL_ERROR "Detected compiler ${CMAKE_CXX_COMPILER_ID}, which is currently not supported. Please report this (or better yet, implement support): https://github.com/usnistgov/NFIQ2/issues")
endif()

if(CMAKE_HOST_WIN32 AND MINGW)
  set(CMAKE_RC_COMPILER_INIT windres)
  ENABLE_LANGUAGE(RC)
  SET(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <DEFINES> -o <OBJECT> <SOURCE>")
endif(CMAKE_HOST_WIN32 AND MINGW)
