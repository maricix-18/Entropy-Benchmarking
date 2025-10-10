@echo off
REM Check if QuEST directory exists
IF NOT EXIST QuEST (
    echo Cloning QuEST V4 repository...
    git clone https://github.com/QuEST-Kit/QuEST.git
) ELSE (
    echo QuEST directory already exists.
)

REM Check if QuEST is already built
cd QuEST
IF EXIST build (
    echo QuEST Project already built.
) ELSE (
    echo Building QuEST V4 Project...
    mkdir build
    cd build
    rem Debug version for now
    cmake ..
    cmake --build .
    cd ..
)
cd ..

REM Get json lib for cpp
IF NOT EXIST json (
    echo Cloning json repository...
    git clone https://github.com/nlohmann/json.git
) ELSE (
    echo json directory already exists.
)


REM Get eigen lib for cpp
IF NOT EXIST eigen (
    echo Cloning eigen repository...
    git clone https://gitlab.com/libeigen/eigen.git
) ELSE (
    echo eigen directory already exists.
)


REM Check if Benchmarking is built
cd Benchmarking
IF EXIST build (
    echo Benchmarking Project already built.
    echo Removing build.
    rmdir /s /q build
)

cd ..

echo All done.

