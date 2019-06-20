set( FRFXLL_PATH ${ROOT_PATH}/fingerjetfxose/FingerJetFXOSE/libFRFXLL )
if(EXISTS "${FRFXLL_PATH}/src/algorithm/version.h")
  FILE(READ "${FRFXLL_PATH}/src/algorithm/version.h" VERSION_H_CONTENT)

  if("${VERSION_H_CONTENT}" MATCHES "#define[ ]+FRFXLL_MAJOR[ ]+\"?([0-9]+)\"?[ ]*\r?\n")
    set( FRFXLL_MAJOR "${CMAKE_MATCH_1}" )
  endif()

  if("${VERSION_H_CONTENT}" MATCHES "#define[ ]+FRFXLL_MINOR[ ]+\"?([0-9]+)\"?[ ]*\r?\n")
    set( FRFXLL_MINOR "${CMAKE_MATCH_1}" )
  endif()

  if("${VERSION_H_CONTENT}" MATCHES "#define[ ]+FRFXLL_REVISION[ ]+\"?([0-9]+)\"?[ ]*\r?\n")
    set( FRFXLL_REVISION "${CMAKE_MATCH_1}" )
  endif()

  set( FRFXLL_VERSION "${FRFXLL_MAJOR}.${FRFXLL_MINOR}.${FRFXLL_REVISION}")
endif()
