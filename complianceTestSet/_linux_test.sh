chmod 777 ./dist/Linux-x86_64/x32/GNU/nfiq2.GNU.linux32.ocv24136.fjfx520.test
chmod 777 ./dist/Linux-x86_64/x64/GNU/nfiq2.GNU.linux64.ocv24136.fjfx520.test

./dist/Linux-x86_64/x32/GNU/nfiq2.GNU.linux32.ocv24136.fjfx520.test BATCH fpImageList.txt BMP result.gnu.linux32.csv true false
./dist/Linux-x86_64/x64/GNU/nfiq2.GNU.linux64.ocv24136.fjfx520.test BATCH fpImageList.txt BMP result.gnu.linux64.csv true false

export LD_LIBRARY_PATH=$(pwd)/dist/Linux-x86_64/x32/GNU/
./dist/Linux-x86_64/x32/GNU/nfiq2.GNU.linux32.ocv24136.fjfx520.test SHARED BMP ./FVC2000_Db1_2_5.bmp ./FVC2000_Db1_4_1.bmp
export LD_LIBRARY_PATH=$(pwd)/dist/Linux-x86_64/x64/GNU/
./dist/Linux-x86_64/x64/GNU/nfiq2.GNU.linux64.ocv24136.fjfx520.test SHARED BMP ./FVC2000_Db1_2_5.bmp ./FVC2000_Db1_4_1.bmp
