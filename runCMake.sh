#!/bin/bash

# @param CMake executable
# @param 1 to show CMake output, 0 otherwise.
# @return Status from cmake
cmake_version_check()
{
   if [ ${#} -ne 2 ]; then
       echo "Usage: ${FUNCNAME} /path/to/cmake <0|1>"
       exit
   fi

   local cmake_exe="${1}"

   wd="$(pwd)"
   tmp_dir="$(mktemp -d)"
   cd "${tmp_dir}"
   if [ "${2}" -eq 0 ]; then
      "${cmake_exe}" "${wd}/cmake/version" &> /dev/null
   else
      "${cmake_exe}" "${wd}/cmake/version" 1> /dev/null
   fi
   local rv=${?}
   cd "${wd}"
   rm -rf "${tmp_dir}"

   return ${rv}
}

# Some distros may have cmake and cmake3
cmake_exe=$(command -v cmake)
cmake3_exe=$(command -v cmake3)
if [ -z "${cmake_exe}" ] && [ -z "${cmake3_exe}" ]; then
      echo "Missing CMAKE installation"
      exit
fi

# Check which, if any, CMake version is correct
correct_cmake_exe=""
if [ -n "${cmake_exe}" ]; then
   cmake_version_check "${cmake_exe}" 0
   if [ ${?} -eq 0 ]; then
      correct_cmake_exe="${cmake_exe}"
   fi
fi
if [ -z "${correct_cmake_exe}" ] && [ -n "${cmake3_exe}" ]; then
   cmake_version_check "${cmake3_exe}" 0
   if [ ${?} -eq 0 ]; then
      correct_cmake_exe="${cmake3_exe}"
   fi
fi
if [ -z "${correct_cmake_exe}" ]; then
   if [ -n "${cmake_exe}" ]; then
      cmake_version_check "${cmake_exe}" 1
   else
      cmake_version_check "${cmake3_exe}" 1
   fi
   exit
fi
cmake_exe="${correct_cmake_exe}"

os="$(uname -s)"
machine="$(uname -m)"
case "${os}" in
    Linux*)     
      machine="Linux-${machine}"
      generator="Unix Makefiles"
      ;;
    Darwin*)    
      machine="Mac-${machine}"
      generator="Unix Makefiles"
      ;;
    CYGWIN*)    
      machine="Cygwin-${machine}"
      generator="Unix Makefiles"
      ;;
    MINGW*)     
      machine="MinGw-${machine}"
      generator="MSYS Makefiles"
      ;;
    *)          
      machine="UNKNOWN:${os}-${machine}"
      echo "Unsupported host system"
      exit
esac

target=$1
if [ "${target}" == "" ]; then
  echo "Missing target (x64, x32, android-arm32, android-arm64, android-x86, android-x64, ios-arm32, ios-arm64)"
  exit
fi
build_folder="./build/${machine}/${target}"

echo "Detected ${machine}, using cmake generator ${generator} for target ${target}"

#linux sanitizer
if [ "${machine:0:5}" == "Linux" ]; then
	if [ "$2" == "sanitizer" ]; then
    build_folder="${build_folder}_sanitizer"
    xtraflags="-DUSE_SANITIZER=ON"
  fi
fi

#android settings (experimental)
if [ "${target:0:7}" == "android" ]; then
  android_ndk=$2
  if [ "${android_ndk}" == "" ]; then
    echo "Missing path to Android NDK"
    exit
  else
    platform="-DANDROID_PLATFORM=android-21" 
    cfg="-DCMAKE_TOOLCHAIN_FILE='${android_ndk}/build/cmake/android.toolchain.cmake'"
    ndk="-DCMAKE_ANDROID_NDK='${android_ndk}'"
    xtraflags="-DANDROID_TARGET=${target}"
  fi
fi

#ios
if [ "${target:0:3}" == "ios" ]; then
	cfg="-DCMAKE_TOOLCHAIN_FILE='./cmake/ios.toolchain.cmake'"
  xtraflags="-DENABLE_ARC=FALSE"
fi

# temporary build directories
mkdir -p "${build_folder}"
cd "${build_folder}"

# run cmake
if [ "${target}" == "x64" ]; then
  ${cmake_exe} -G "${generator}" -D32BITS=OFF -D64BITS=ON "$xtraflags" ../../../
elif [ "${target}" == "x32" ]; then
  ${cmake_exe} -G "${generator}" -D32BITS=ON -D64BITS=OFF "$xtraflags" ../../../
elif [ "${target}" == "android-arm32" ]; then
  ${cmake_exe} -G "$generator" "$ndk" "$platform" -DANDROID_ABI=armeabi-v7a "$cfg" "$xtraflags" "../../../"
elif [ "${target}" == "android-arm64" ]; then
  ${cmake_exe} -G "$generator" "$ndk" "$platform" -DANDROID_ABI=arm64-v8a "$cfg" "$xtraflags" "../../../"
elif [ "${target}" == "android-x86" ]; then
  ${cmake_exe} -G "$generator" "$ndk" "$platform" -DANDROID_ABI=x86 "$cfg" "$xtraflags" "../../../"
elif [ "${target}" == "android-x64" ]; then
  ${cmake_exe} -G "$generator" "$ndk" "$platform" -DANDROID_ABI=x86_64 "$cfg" "$xtraflags" "../../../"
elif [ "${target}" == "ios-arm32" ]; then
  ${cmake_exe} -G "$generator" -DPLATFORM=OS "$cfg" "$xtraflags" "../../../"
elif [ "${target}" == "ios-arm64" ]; then
  ${cmake_exe} -G "$generator" -DPLATFORM=OS64 "$cfg" "$xtraflags" "../../../"
else
  echo "Missing target (x64, x32, android-arm, android-arm64)"
  exit
fi

# cleanup
cd ..



