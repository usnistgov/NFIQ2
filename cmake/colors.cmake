execute_process(
  COMMAND pwd
  OUTPUT_VARIABLE PWD_OUT
)

if( NOT ${PWD_OUT} MATCHES ":")
  string(ASCII 27 Esc)
  set(Na "${Esc}[m")
  set(Rd "${Esc}[31m")
  set(Gn "${Esc}[32m")
  set(Ye "${Esc}[33m")
  set(Bl "${Esc}[34m")
  set(Pk "${Esc}[35m")
  set(Gy "${Esc}[37m")
else()
  message( STATUS "Running in PowerShell, do not use colors")
endif()
