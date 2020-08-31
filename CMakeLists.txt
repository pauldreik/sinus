cmake_minimum_required(VERSION 3.10)

project(sinus_benchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(googletest)
add_subdirectory(benchmark)



add_executable(bench benchmark.cpp
    VectorLibrary.cpp
    VectorLibrary.hpp
    StandardLibrary.hpp
    StandardLibrary.cpp
    NoOpLib.cpp)
target_include_directories(bench PRIVATE vectorclass)
target_compile_options(bench PRIVATE -mavx2 -mfma -mtune=native)
target_compile_options(bench PRIVATE -fno-trapping-math -fno-math-errno)
target_link_libraries(bench PRIVATE benchmark::benchmark)

# warnings
target_compile_options(bench PRIVATE -Wall)

# enable this for sanitizer support
#target_compile_options(bench PRIVATE -fsanitize=address)
#target_link_options(bench PRIVATE -fsanitize=address)
