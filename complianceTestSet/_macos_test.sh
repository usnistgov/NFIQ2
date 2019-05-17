chmod 777 ./dist/Mac-x86_64/x32/AppleClang/nfiq2.AppleClang.apple32.ocv24136.fjfx520.test
chmod 777 ./dist/Mac-x86_64/x64/AppleClang/nfiq2.AppleClang.apple64.ocv24136.fjfx520.test

./dist/Mac-x86_64/x32/AppleClang/nfiq2.AppleClang.apple32.ocv24136.fjfx520.test BATCH fpImageList.txt BMP result.AppleClang.apple32.csv true false
./dist/Mac-x86_64/x64/AppleClang/nfiq2.AppleClang.apple64.ocv24136.fjfx520.test BATCH fpImageList.txt BMP result.AppleClang.apple64.csv true false

export LD_LIBRARY_PATH=$(pwd)/dist/Mac-x86_64/x32/AppleClang/
./dist/Mac-x86_64/x32/AppleClang/nfiq2.AppleClang.apple32.ocv24136.fjfx520.test SHARED BMP ./FVC2000_Db1_2_5.bmp ./FVC2000_Db1_4_1.bmp

export LD_LIBRARY_PATH=$(pwd)/dist/Mac-x86_64/x64/AppleClang/
./dist/Mac-x86_64/x64/AppleClang/nfiq2.AppleClang.apple64.ocv24136.fjfx520.test SHARED BMP ./FVC2000_Db1_2_5.bmp ./FVC2000_Db1_4_1.bmp

