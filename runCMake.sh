#!/bin/bash

test=`command -v cmake`

if [[ -z "${test}" ]]; then
   echo "Missing CMAKE installation"
   exit
fi

test="$(uname -s)"
machine="$(uname -m)"
case "${test}" in
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
      machine="UNKNOWN:${unameOut}-${machine}"
      echo "Unsupported host system"
      exit
esac

target=$1
if [ "${target}" == "" ]; then
  echo "Missing target (x64, x32, android-arm32, android-arm64, ios-arm32, ios-arm64)"
  exit
fi

echo "Detected ${machine}, using cmake generator ${generator} for target ${target}"

# temporary build directories
mkdir -p "./build/${machine}/${target}"
cd "./build/${machine}/${target}"

#android settings (experimental)
if [ ${target:0:7} == "android" ]; then
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

if [ ${target:0:3} == "ios" ]; then
	cfg="-DCMAKE_TOOLCHAIN_FILE='./nfiq2/ios.toolchain.cmake'"
    xtraflags="-DENABLE_ARC=FALSE"
fi

# run cmake
if [ "${target}" == "x64" ]; then
  cmake -G "${generator}" -D32BITS=OFF -D64BITS=ON ../../../
elif [ "${target}" == "x32" ]; then
  cmake -G "${generator}" -D32BITS=ON -D64BITS=OFF ../../../
elif [ "${target}" == "android-arm32" ]; then
  cmake -G "$generator" "$ndk" "$platform" -DANDROID_ABI=armeabi-v7a "$cfg" "$xtraflags" "../../../"
elif [ "${target}" == "android-arm64" ]; then
  cmake -G "$generator" "$ndk" "$platform" -DANDROID_ABI=arm64-v8a "$cfg" "$xtraflags" "../../../"
elif [ "${target}" == "ios-arm32" ]; then
  cmake -G "$generator" -DPLATFORM=OS "$cfg" "$xtraflags" "../../../"
elif [ "${target}" == "ios-arm64" ]; then
  cmake -G "$generator" -DPLATFORM=OS64 "$cfg" "$xtraflags" "../../../"
else
  echo "Missing target (x64, x32, android-arm, android-arm64)"
  exit
fi

# cleanup
cd ..



