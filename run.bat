REM run script will rebuild only the benchmark project
cd Benchmarking
rmdir /s /q build
del CMakeCache.txt

echo Building Benchmarking Project...
mkdir build
cd build
cmake ..

msbuild entropy_benchmarking.sln /t:Rebuild
Debug\entropy_benchmarking.exe
cd ..
cd ..