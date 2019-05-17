@echo off
call .\dist\MinGw-x86_64\x32\GNU\nfiq2.GNU.win32.ocv24136.fjfx520.test.exe BATCH fpImageList.txt BMP result.gnu.win32.csv true false
call .\dist\MinGw-x86_64\x64\GNU\nfiq2.GNU.win64.ocv24136.fjfx520.test.exe BATCH fpImageList.txt BMP result.gnu.win64.csv true false
call .\dist\Windows32\MSVC\Release\nfiq2.MSVC.win32.ocv24136.fjfx520.test.exe BATCH fpImageList.txt BMP result.msvc.win32.csv true false
call .\dist\Windows64\MSVC\Release\nfiq2.MSVC.win64.ocv24136.fjfx520.test.exe BATCH fpImageList.txt BMP result.msvc.win64.csv true false

call .\dist\MinGw-x86_64\x32\GNU\nfiq2.GNU.win32.ocv24136.fjfx520.test.exe SHARED BMP ./FVC2000_Db1_2_5.bmp ./FVC2000_Db1_4_1.bmp
call .\dist\MinGw-x86_64\x64\GNU\nfiq2.GNU.win64.ocv24136.fjfx520.test.exe SHARED BMP ./FVC2000_Db1_2_5.bmp ./FVC2000_Db1_4_1.bmp
call .\dist\Windows32\MSVC\Release\nfiq2.MSVC.win32.ocv24136.fjfx520.test.exe SHARED BMP ./FVC2000_Db1_2_5.bmp ./FVC2000_Db1_4_1.bmp
call .\dist\Windows64\MSVC\Release\nfiq2.MSVC.win64.ocv24136.fjfx520.test.exe SHARED BMP ./FVC2000_Db1_2_5.bmp ./FVC2000_Db1_4_1.bmp


