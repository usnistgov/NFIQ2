option(USE_SANITIZER "Using the GCC sanitizer" OFF)
include(CheckCXXSourceRuns)

add_definitions("-D_USE_MATH_DEFINES")
if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))
#####################################################
  message( STATUS "${Gn}Detected ${CMAKE_CXX_COMPILER_ID} compiler${Na}" )
  if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
    message( STATUS "${Gn}Detected Android${Na}" )
    set( LINK_LIB_PREFIX "${BUILD_PATH}/opencv-${OPENCV_VERSION}/lib/${ANDROID_ABI}/lib")
    set( LINK_3RD_PREFIX "${BUILD_PATH}/opencv-${OPENCV_VERSION}/3rdparty/lib/${ANDROID_ABI}/lib")
  else()
    set( LINK_LIB_PREFIX "${BUILD_PATH}/opencv-${OPENCV_VERSION}/lib/lib")
    set( LINK_3RD_PREFIX "${BUILD_PATH}/opencv-${OPENCV_VERSION}/3rdparty/lib/lib")
  endif()
##################################################
  if( USE_SANITIZER )
    message( STATUS "${Pk}Configure diagnostic build with sanitizer${Na}")
    add_definitions( "-O0 -Wno-unused-variable -fsanitize=address -fsanitize=leak -fstack-check -fno-omit-frame-pointer -static-libasan" )
  else()
    add_definitions( "-O3 -Wno-unused-variable " )
  endif()
  if("${TARGET_PLATFORM}" MATCHES "win*")
    add_definitions("-DWIN32 -D_WIN32")
  else()
    add_definitions("-DLINUX -fPIC")
  endif()

  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_definitions("-Wno-unused-but-set-variable ")
    if("${TARGET_PLATFORM}" MATCHES "win*")
      set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static -static-libgcc -static-libstdc++ -Wl,-add-stdcall-alias -Wl,-enable-stdcall-fixup")
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++ -Wl,-enable-stdcall-fixup")
      check_cxx_source_runs("int main() { return 0; }" STATIC_SYSTEM_LIBS)
      if (NOT STATIC_SYSTEM_LIBS)
         message(WARNING "You are missing one or more static system libraries that will be linked by -static-libgcc -static-libstdc++")
      endif()
    elseif("${TARGET_PLATFORM}" MATCHES "linux*")
      set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc -static-libstdc++ -Wl,-z,defs")
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
      check_cxx_source_runs("int main() { return 0; }" STATIC_SYSTEM_LIBS)
      if (NOT STATIC_SYSTEM_LIBS)
         message(WARNING "You are missing one or more static system libraries that will be linked by -static-libgcc -static-libstdc++")
      endif()
    endif()
  endif()

  if( 32BITS)
    set( CMAKE_C_FLAGS "-m32")
    set( CMAKE_CXX_FLAGS "-m32")
  elseif( 64BITS)
    set( CMAKE_C_FLAGS "-m64")
    set( CMAKE_CXX_FLAGS "-m64")
  endif()

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  message( STATUS "${Gn}Detected MSVC compiler (Windows)${Na}" )
  set( LINK_LIB_PREFIX "${BUILD_PATH}/opencv-${OPENCV_VERSION}/lib/Release/")
  set( LINK_3RD_PREFIX "${BUILD_PATH}/opencv-${OPENCV_VERSION}/3rdparty/lib/Release/")

  # Static-link MS CRT
    foreach(flag_var
            CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
#      if (DEBUG)
#      set(${flag_var} "${${flag_var}} /MDd /nologo")
#      else()
      set(${flag_var} "${${flag_var}} /MD /nologo")
#      endif()
    endforeach(flag_var)
#  if (STATIC_LINK)
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
#  endif()

  # Disable some "unsafe" warnings
  foreach(flag_var
          CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
          CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
      set(${flag_var} "${${flag_var}} /D_CRT_SECURE_NO_WARNINGS /nologo")
  endforeach(flag_var)

  # Don't show copyright
  foreach(flag_var
          CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
          CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
      set(${flag_var} "${${flag_var}} /nologo")
  endforeach(flag_var)

  # Show most warnings
  foreach(flag_var
          CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
          CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
      set(${flag_var} "${${flag_var}} /W3")
  endforeach(flag_var)

else()
  message( STATUS "${Rd}Detected compiler ${CMAKE_CXX_COMPILER_ID} which is currently not supported${Na}" )
endif()

if(CMAKE_HOST_WIN32 AND MINGW)
  set(CMAKE_RC_COMPILER_INIT windres)
  ENABLE_LANGUAGE(RC)
  SET(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <DEFINES> -o <OBJECT> <SOURCE>")
endif(CMAKE_HOST_WIN32 AND MINGW)
