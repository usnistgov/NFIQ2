# NFIQ2
> This is a rework of [NFIQ2](https://github.com/usnistgov/NFIQ2).

# Referência
>
National Institute of Standards and Technology (NIST) Fingerprint Image Quality (NFIQ). 
(https://github.com/usnistgov/NFIQ2).

### 1.1 Obtendo o código fonte do projeto:
```   
cd $env:userprofile ; git clone --recursive https://github.com/usnistgov/NFIQ2.git
cd NFIQ2 ; git checkout -b nfiq2-devel ; mkdir build
```

### 1.2 Instalando o gerenciador de pacotes no Windows:
```   
mkdir $env:homedrive\dev ; cd $env:homedrive\dev
git clone https://github.com/Microsoft/vcpkg.git ; .\vcpkg\bootstrap-vcpkg.bat
vcpkg integrate install
```

### 1.3 Instalando as dependências do projeto:
```
cd $env:homedrive\dev\vcpkg\ `
.\vcpkg.exe install openssl:x86-windows `
openssl:x64-windows `
openjpeg:x86-windows `
openjpeg:x64-windows `
libjpeg-turbo:x86-windows `
libjpeg-turbo:x64-windows `
getopt:x86-windows `
getopt:x64-windows `
libpng:x86-windows `
libpng:x64-windows `
tiff:x86-windows `
tiff:x64-windows `
zlib:x86-windows `
zlib:x64-windows `
msmpi:x86-windows `
msmpi:x64-windows `
berkeleydb:x86-windows `
berkeleydb:x64-windows `
sqlite3:x86-windows `
sqlite3:x64-windows `
hwloc:x86-windows `
hwloc:x64-windows `
ffmpeg:x86-windows `
ffmpeg:x64-windows `
```

### 1.4 Configurando as variavéis de ambiente no Windows:

#### 1.4.1 Configurando as variavéis de usuário:

```
setx CMAKE_CONFIGURATION_TYPES "Release"
setx CMAKE_TOOLCHAIN_FILE "%HOMEDRIVE%\dev\vcpkg\scripts\buildsystems\vcpkg.cmake"
setx VCPKG_TARGET_TRIPLET "x64-windows-static"
```

#### 1.4.2 Configurando as variavéis de sistema:
```
setx CMAKE_CONFIGURATION_TYPES "Release" /m
setx CMAKE_TOOLCHAIN_FILE "%HOMEDRIVE%\dev\vcpkg\scripts\buildsystems\vcpkg.cmake" /m
setx VCPKG_TARGET_TRIPLET "x64-windows-static" /m
```

### 1.5 Compilando com suporte para as arquiteturas x64 e x86:

#### 1.5.1 Compilação para arquitetura: x64:
```
cd $env:userprofile\NFIQ2\build ; cmake .. -A x64 `
-D BUILD_NFIQ2_CLI=OFF `
-D BUILD_SHARED_LIBS=FALSE `
-D CMAKE_CXX_FLAGS_RELEASE="/MT /MTd" `
-D CMAKE_CXX_FLAGS="/MT /MTd" `
-D CMAKE_C_FLAGS="/MT /MTd" `
-D CMAKE_C_FLAGS_RELEASE="/MT /MTd" `
```
```
cmake --build . --config Release
```

#### 1.5.2 Compilação para arquitetura: x86:
```
cd $env:userprofile\NFIQ2\build ; cmake .. -A Win32 `
-D BUILD_NFIQ2_CLI=OFF `
-D BUILD_SHARED_LIBS=FALSE `
-D CMAKE_CXX_FLAGS_RELEASE="/MT /MTd" `
-D CMAKE_CXX_FLAGS="/MT /MTd" `
-D CMAKE_C_FLAGS="/MT /MTd" `
-D CMAKE_C_FLAGS_RELEASE="/MT /MTd" `
```
```
cmake --build . --config Release  
```

### 1.6 Modificando o projeto para utilizar bibliotecas e pacotes estáticos:

```
set(OpenCV_DIR "C:/opencv_static_4_7_0/x64-Release/x64/vc17/staticlib")
find_package(OpenCV REQUIRED core imgproc imgcodecs videoio ml highgui)
set(OpenCV_STATIC ON)
include_directories(${OpenCV_INCLUDE_DIRS})
```

### 1.7 Compilando o código de exemplo: 
```
cmake .. `
-D LIBNFIQ2_STATIC=$env:userprofile\NFIQ2\build\install_staging\nfiq2\lib\nfiq2.lib `
-D LIBFRFXLL_STATIC=$env:userprofile\NFIQ2\build\install_staging\nfiq2\lib\FRFXLL_static.lib `
-D OpenCV_DIR="C:\opencv_static_4_7_0\x64-Release\" `
-D NFIQ2_INSTALL_DIR="$env:userprofile\NFIQ2\build\install_staging\nfiq2\include" `
```
```
cmake --build . --config Release
```

### 1.8 Exemplo de execução utilizando à linha de comando:

```
.\Release\example_api.exe $env:userprofile\NFIQ2\NFIQ2\nist_plain_tir-ink.txt $env:userprofile\NFIQ2\examples\images\SFinGe_Test01.pgm
```