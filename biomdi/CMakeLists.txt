project( biomdi ) 

# download sources
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/download/)
  set( BIOMDI_SRC "https://nigos.nist.gov:8080/nist/biomdi/biomdi_current.zip")
  set( BIOMDI_DST "${CMAKE_CURRENT_SOURCE_DIR}/download/tmp.zip")
  message("Download ${BIOMDI_SRC} to ${BIOMDI_DST}")
  file(DOWNLOAD "${BIOMDI_SRC}" "${BIOMDI_DST}")
  file(SIZE "${BIOMDI_DST}" BIOMDI_SIZE)
  message("Downloaded ${BIOMDI_SIZE} bytes")
  execute_process( COMMAND cmake -E tar xfz tmp.zip WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/download/ )
  file(REMOVE "${BIOMDI_DST}")
endif()

set(BIOMDI_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/download//BIOMDI/Main")

# add include directories
include_directories("${BIOMDI_ROOT}/common/src/include")
if("${TARGET_PLATFORM}" MATCHES "win*")
  include_directories("${CMAKE_CURRENT_SOURCE_DIR}/windows")
endif()

set(SOURCE_FILES "")
file( GLOB SOURCE_FILES "${BIOMDI_ROOT}/common/src/libbiomdi/*.c" ) 
add_library( "biomdi" STATIC
  ${SOURCE_FILES}
)

# FACE RECOGNITION ##############################################

include_directories("${BIOMDI_ROOT}/facerecognition/src/include")
set(SOURCE_FILES "")
file( GLOB SOURCE_FILES "${BIOMDI_ROOT}/facerecognition/src/libfrf/*.c" ) 
add_library( "frf" STATIC
  ${SOURCE_FILES}
)

function(facerecognition exec_name)
  set(SOURCE_FILES "")
  file( GLOB SOURCE_FILES "${BIOMDI_ROOT}/facerecognition/src/${exec_name}/*.c" "${BIOMDI_ROOT}/facerecognition/src/${exec_name}/*.cpp" ) 
  add_executable( ${exec_name}
    ${SOURCE_FILES}
  )
  link_directories(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
  if("${TARGET_PLATFORM}" MATCHES "win*")
    target_link_libraries( ${exec_name} "frf" "biomdi" "ws2_32") 
  else()
    target_link_libraries( ${exec_name} "frf" "biomdi" ) 
  endif()
endfunction()

#facerecognition( "cpfrf" )
#facerecognition( "frfv" )
#facerecognition( "mkfrf" )
#facerecognition( "prfrf" )

# FINGER IMAGE ##################################################

include_directories("${BIOMDI_ROOT}/fingerimage/src/include")
set(SOURCE_FILES "")
file( GLOB SOURCE_FILES "${BIOMDI_ROOT}/fingerimage/src/libfir/*.c" ) 
add_library( "fir" STATIC
  ${SOURCE_FILES}
)

function(fingerimage exec_name)
  set(SOURCE_FILES "")
  file( GLOB SOURCE_FILES "${BIOMDI_ROOT}/fingerimage/src/${exec_name}/*.c" "${BIOMDI_ROOT}/fingerimage/src/${exec_name}/*.cpp" ) 
  add_executable( ${exec_name}
    ${SOURCE_FILES}
  )
  link_directories(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
  if("${TARGET_PLATFORM}" MATCHES "win*")
    target_link_libraries( ${exec_name} "fir" "biomdi" "ws2_32") 
  else()
    target_link_libraries( ${exec_name} "fir" "biomdi" ) 
  endif()
endfunction()

#fingerimage("firv")
#fingerimage("mkfir")
##fingerimage("pgm2fir") # does not build due pgm.h
#fingerimage("prfir")

# FINGER MINUTIA ################################################

include_directories("${BIOMDI_ROOT}/fingerminutia/src/include")
set(SOURCE_FILES "")
file( GLOB SOURCE_FILES "${BIOMDI_ROOT}/fingerminutia/src/libfmr/*.c" ) 
add_library( "fmr" STATIC
  ${SOURCE_FILES}
)

function(fingerminutia exec_name)
  set(SOURCE_FILES "")
  file( GLOB SOURCE_FILES "${BIOMDI_ROOT}/fingerminutia/src/${exec_name}/*.c" "${BIOMDI_ROOT}/fingerminutia/src/${exec_name}/*.cpp" ) 
  add_executable( ${exec_name}
    ${SOURCE_FILES}
  )
  link_directories(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
  if("${TARGET_PLATFORM}" MATCHES "win*")
    target_link_libraries( ${exec_name} "fmr" "biomdi" "ws2_32") 
  else()
    target_link_libraries( ${exec_name} "fmr" "biomdi" "m" ) 
  endif()
endfunction()

##fingerminutia("an2k2fmr")  # does not build due an2k.h
##fingerminutia("fmr2an2k")  # does not build due an2k.h
#fingerminutia("fmr2fmr")
#fingerminutia("fmrisocompact")
#fingerminutia("fmrmod")
#fingerminutia("fmroverlap")
##fingerminutia("fmrplot")  # does not build due gd.h
#fingerminutia("fmrprune")
#fingerminutia("fmrsort")
#fingerminutia("fmrv")
##fingerminutia("minexv")  # does not build due MAX_REC_LENGTH 
#fingerminutia("mkfmr")
#fingerminutia("prfmr")

# IRIS IMAGE ####################################################

include_directories("${BIOMDI_ROOT}/irisimage/src/include")
set(SOURCE_FILES "")
file( GLOB SOURCE_FILES "${BIOMDI_ROOT}/irisimage/src/libiid/*.c" ) 
add_library( "iid" STATIC
  ${SOURCE_FILES}
)

function(irisimage exec_name)
  set(SOURCE_FILES "")
  file( GLOB SOURCE_FILES "${BIOMDI_ROOT}/irisimage/src/${exec_name}/*.c" "${BIOMDI_ROOT}/irisimage/src/${exec_name}/*.cpp" ) 
  add_executable( ${exec_name}
    ${SOURCE_FILES}
  )
  link_directories(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
  if("${TARGET_PLATFORM}" MATCHES "win*")
    target_link_libraries( ${exec_name} "iid" "biomdi" "ws2_32") 
  else()
    target_link_libraries( ${exec_name} "iid" "biomdi" ) 
  endif()
endfunction()

#irisimage("iibdbv")
#irisimage("priibdb")
