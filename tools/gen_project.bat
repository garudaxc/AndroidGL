@ echo off

del /s /q ..\Build
rd /s /q ..\Build
mkdir ..\Build

@ set PATH=D:\Program Files\cmake-3.16.3-win64-x64\bin;%PATH%

cmake . -B ..\Build -A Win32

pause