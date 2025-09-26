@echo off
if exist Digitaler.exe del Digitaler.exe
if exist Error.log del Error.log

if not exist "..\build" (
    mkdir "..\build"
    pushd "..\build"
    cmake -DCMAKE_BUILD_TYPE=Release ..
    popd
)

cmake --build ..\build --parallel %NUMBER_OF_PROCESSORS%
if errorlevel 1 (
    cmake -DCMAKE_BUILD_TYPE=Release -S .. -B ..\build
    cmake --build ..\build --parallel %NUMBER_OF_PROCESSORS%
)

.\Digitaler.exe
