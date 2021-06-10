# Version information
set(NFIQ2_VERSION_MAJOR "2")
set(NFIQ2_VERSION_MINOR "1")
set(NFIQ2_VERSION_PATCH "0")
set(NFIQ2_VERSION_STATUS "")

################################################################################
#                     DO NOT CHANGE VALUES BELOW THIS LINE                     #
################################################################################

#
# Product metadata
#

set(NFIQ2_PRODUCT_NAME "NFIQ 2")
set(NFIQ2_PRODUCT_DESCRIPTION_SHORT "NIST Fingerprint Image Quality 2 algorithm")
set(NFIQ2_PRODUCT_VENDOR "National Institute of Standards and Technology (NIST) and contributors")
set(NFIQ2_EMAIL "nfiq2@nist.gov")

#
# Build metadata
#

# Date of build
string(TIMESTAMP NFIQ2_BUILD_DATE "%Y%m%d%H%M")
set(NFIQ2_VERSION_BUILDMETADATA "${NFIQ2_BUILD_DATE}")

# git HEAD hash
set(NFIQ2_VERSION_COMMIT "")
if(NOT DEFINED NFIQ2_VERSION_BUILD OR "${NFIQ2_VERSION_BUILD}" STREQUAL "")
	execute_process(COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE gitOut ERROR_VARIABLE gitErr)
	if("${gitErr}" STREQUAL "" AND NOT "${gitOut}" STREQUAL "")
		string(REGEX REPLACE "(\r?\n)+$" "" NFIQ2_VERSION_COMMIT "${gitOut}")
	else()
		message(WARNING "Could not retrieve git hash")
		set(NFIQ2_VERSION_COMMIT "XXXXXXX")
	endif()
endif()
if (NOT "${NFIQ2_VERSION_COMMIT}" STREQUAL "")
	set(NFIQ2_VERSION_BUILDMETADATA "${NFIQ2_VERSION_BUILDMETADATA}.${NFIQ2_VERSION_COMMIT}")
endif()


################################################################################

set(NFIQ2_VERSION "${NFIQ2_VERSION_MAJOR}.${NFIQ2_VERSION_MINOR}.${NFIQ2_VERSION_PATCH}")
set(NFIQ2_VERSION_FULL "${NFIQ2_VERSION}")

# Include more info if a pre-release version
if (NOT "${NFIQ2_VERSION_STATUS}" STREQUAL "")
	set(NFIQ2_VERSION "${NFIQ2_VERSION_FULL}-${NFIQ2_VERSION_STATUS}")
	set(NFIQ2_VERSION_FULL "${NFIQ2_VERSION_FULL}-${NFIQ2_VERSION_STATUS}+${NFIQ2_VERSION_BUILDMETADATA}")
endif()

################################################################################

if("${TARGET_PLATFORM}" MATCHES "linux*")
  set( VERSION_VISIBILITY_ATTRIBUTE "__attribute__( ( section( \".nfiq2_version_info\" ) ) ) __attribute__( ( visibility( \"default\" ) ) )")
else()
  set( VERSION_VISIBILITY_ATTRIBUTE "")
endif()

set(VERSION_SUFFIX "${CMAKE_CXX_COMPILER_ID}.${TARGET_PLATFORM}")

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/version.cpp.in")
  configure_file(version.cpp.in ${CMAKE_CURRENT_SOURCE_DIR}/version.${VERSION_SUFFIX}.cpp)
  set(VERSION_FILES ${CMAKE_CURRENT_SOURCE_DIR}/version.${VERSION_SUFFIX}.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in")
    configure_file(version.rc.in ${CMAKE_CURRENT_SOURCE_DIR}/version.${VERSION_SUFFIX}.rc)
    set(VERSION_FILES ${VERSION_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/version.${VERSION_SUFFIX}.rc)
endif()
