
cd Benchmarking
rmdir /s /q build
del CMakeCache.txt

set PATH=C:\Users\maria\Desktop\Entropy_Benchmark_quest\Entropy-Benchmarking\QuEST\build;%PATH%

mkdir build
cd build
cmake ..
msbuild entropy_benchmarking.sln /t:Rebuild
Debug\entropy_benchmarking.exe

cd ..
cd ..
