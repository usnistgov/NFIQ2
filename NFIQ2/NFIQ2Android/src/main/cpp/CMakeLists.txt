cmake_minimum_required(VERSION 3.28)
project(nfiq2_jni LANGUAGES CXX)

option(NFIQ2_EMBED_MODEL "NFIQ2 was built with embedded model parameters" OFF)

if(NOT EXISTS "${NFIQ2_INSTALL_DIR}/lib/libnfiq2.a")
    message(FATAL_ERROR "NFIQ2 is missing. Run the Gradle buildNfiq2 task first.")
endif()
find_package(OpenCV REQUIRED COMPONENTS core imgproc ml
    PATHS "${NFIQ2_INSTALL_DIR}/sdk/native/jni"
    NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
add_library(nfiq2 STATIC IMPORTED)
set_target_properties(nfiq2 PROPERTIES
    IMPORTED_LOCATION "${NFIQ2_INSTALL_DIR}/lib/libnfiq2.a"
    INTERFACE_INCLUDE_DIRECTORIES "${NFIQ2_INSTALL_DIR}/include")
add_library(fingerjet STATIC IMPORTED)
set_target_properties(fingerjet PROPERTIES
    IMPORTED_LOCATION "${NFIQ2_INSTALL_DIR}/lib/libFRFXLL_static.a")
add_library(nfiq2_jni SHARED nfiq2_jni.cpp)
target_compile_features(nfiq2_jni PRIVATE cxx_std_11)
if(NFIQ2_EMBED_MODEL)
    target_compile_definitions(nfiq2_jni PRIVATE
        NFIQ2_EMBED_RANDOM_FOREST_PARAMETERS
        NFIQ2_EMBEDDED_RANDOM_FOREST_PARAMETERS_FCT=0)
endif()
target_link_libraries(nfiq2_jni PRIVATE nfiq2 fingerjet ${OpenCV_LIBS} android log)
target_link_options(nfiq2_jni PRIVATE "-Wl,-z,max-page-size=16384")
