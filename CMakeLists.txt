cmake_minimum_required(VERSION 3.10)

project(sinus_benchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(googletest)
add_subdirectory(benchmark)



add_library(benchcommon
    VectorLibrary.cpp
    VectorLibrary.hpp
    StandardLibrary.hpp
    StandardLibrary.cpp
    NoOpLib.cpp)
target_include_directories(benchcommon PUBLIC vectorclass)
target_compile_options(benchcommon PUBLIC -mavx2 -mfma -mtune=native)
target_compile_options(benchcommon PUBLIC -fno-trapping-math -fno-math-errno)
target_link_libraries(benchcommon PUBLIC benchmark::benchmark)

# warnings
target_compile_options(benchcommon PUBLIC -Wall)

# enable this for sanitizer support
#target_compile_options(bench PRIVATE -fsanitize=address)
#target_link_options(bench PRIVATE -fsanitize=address)

add_executable(benchfloat benchmark.cpp)
target_link_libraries(benchfloat PRIVATE benchcommon)
target_compile_definitions(benchfloat PRIVATE FLOATTYPE=float)


add_executable(benchdouble benchmark.cpp)
target_link_libraries(benchdouble PRIVATE benchcommon)
target_compile_definitions(benchdouble PRIVATE FLOATTYPE=double)

add_executable(cmdline cmdline.cpp)
target_link_libraries(cmdline PRIVATE benchcommon)
