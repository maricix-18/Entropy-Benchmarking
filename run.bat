REM run script will rebuild only the benchmark project
REM if in debug mode
set "RELATIVE_PATH=QuEST\build\Debug"
set "FULL_PATH=%CD%\%RELATIVE_PATH%"

REM Check if the directory exists
if exist "%FULL_PATH%" (
    echo Adding %FULL_PATH% to PATH...
    set "PATH=%FULL_PATH%;%PATH%"
) else (
    echo Directory does not exist: %FULL_PATH%
)

cd Benchmarking
rmdir /s /q build
del CMakeCache.txt

echo Building Benchmarking Project...
mkdir build
cd build
cmake ..
cmake --build .

REM currently in debug mode

cd Debug
entropy_benchmarking.exe

cd ..
cd ..
cd ..