cmake_minimum_required(VERSION 3.10)

project(untitled LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(benchmark)

add_executable(untitled main.cpp VectorLibrary.cpp VectorLibrary.hpp)
target_include_directories(untitled PRIVATE vectorclass)
target_compile_options(untitled PRIVATE -mavx2 -mfma -mtune=native)
target_compile_options(untitled PRIVATE -fno-trapping-math -fno-math-errno)

# warnings
target_compile_options(untitled PRIVATE -Wall)

# enable this for sanitizer support
#target_compile_options(untitled PRIVATE -fsanitize=address)
#target_link_options(untitled PRIVATE -fsanitize=address)


add_executable(bench benchmark.cpp  VectorLibrary.cpp  VectorLibrary.hpp)
target_include_directories(bench PRIVATE vectorclass)
#target_include_directories(bench PRIVATE benchmark/include)
target_compile_options(bench PRIVATE -mavx2 -mfma -mtune=native)
target_compile_options(bench PRIVATE -fno-trapping-math -fno-math-errno)
target_link_libraries(bench PRIVATE benchmark::benchmark)
