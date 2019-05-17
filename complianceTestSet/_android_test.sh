chmod 777 ./dist/MinGw-x86_64/android-arm32/Clang/nfiq2.Clang.android-arm32.ocv24136.fjfx520.test
chmod 777 ./dist/MinGw-x86_64/android-arm64/Clang/nfiq2.Clang.android-arm64.ocv24136.fjfx520.test
./dist/MinGw-x86_64/android-arm32/Clang/nfiq2.Clang.android-arm32.ocv24136.fjfx520.test BATCH fpImageList.txt BMP result.clang.android-arm32.csv true false
./dist/MinGw-x86_64/android-arm64/Clang/nfiq2.Clang.android-arm64.ocv24136.fjfx520.test BATCH fpImageList.txt BMP result.clang.android-arm64.csv true false

export LD_LIBRARY_PATH=$(pwd)/dist/MinGw-x86_64/android-arm32/Clang/
./dist/MinGw-x86_64/android-arm32/Clang/nfiq2.Clang.android-arm32.ocv24136.fjfx520.test SHARED BMP ./FVC2000_Db1_2_5.bmp ./FVC2000_Db1_4_1.bmp
export LD_LIBRARY_PATH=$(pwd)/dist/MinGw-x86_64/android-arm64/Clang/
./dist/MinGw-x86_64/android-arm64/Clang/nfiq2.Clang.android-arm64.ocv24136.fjfx520.test SHARED BMP ./FVC2000_Db1_2_5.bmp ./FVC2000_Db1_4_1.bmp
