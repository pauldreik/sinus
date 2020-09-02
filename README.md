# Sinus

This is a repo with code for experimenting with calculating sin() and cos().

The idea is to see if it is possible to trade of accuracy for speed, and learn about the
characteristics of different methods.

So far, I have learned that the calculation speed is data dependent.

## Contenders
The standard library std::sin() is obviously the reference to compare to.

To use SIMD, [vectorclass by Agner Fog](https://github.com/vectorclass/version2) is also used, called vectorlib in the results table.

There is also nooplib which does nothing, just for measuring the benchmarking overhead.

## Building the code and benchmark
You need a C++17 capable compiler and cmake.

```shell
git clone https://github.com/pauldreik/sinus/
cd sinus/
git submodule update --init
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j 4 
./benchfloat
./benchdouble
```

## Test data
Since I do this with a specific application in mind, I try to mimick the application as close as possible.

An array of 3000 elements (float or double) is filled. The elements are replaced with the output of the sin() function.

The data can be
 - uniformly random within a tiny range +-1e-30
 - uniformly random within a medium range +-100
 - linearly increasing from 0 to 100pi
 - binary garbage

## Benchmark (CPU consumption)
The data below are obtained on a 3 GHz 2018 AMD desktop cpu, running gcc 9.3 in Ubuntu 20.04

The conclusion is that 

 - performance is data dependent
 - binary garbage is expensive
 - vectorlib is faster than stdlib for realistic inputs
 
 It is not strange that glibc (invoked by the standard library) has
 data dependent performance, since it switches between different modes
 depending on the input. I would however guess that vectorlib had
 constant performance.

## Performance for sin(float)
```
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
nooplib/overhead_measure_tiny        189 ns          189 ns      3665550 elements per second=10.9966G
vectorlib/tiny                      8123 ns         8110 ns        84714 elements per second=254.142M
vectorlib/medium                    3371 ns         3367 ns       204328 elements per second=612.984M
vectorlib/increasing                3360 ns         3356 ns       206307 elements per second=618.921M
vectorlib/garbage                   6364 ns         6350 ns       108203 elements per second=324.609M
stdlib/tiny                         6245 ns         6242 ns       100109 elements per second=300.327M
stdlib/medium                      15773 ns        15672 ns        44984 elements per second=134.952M
stdlib/increasing                  15070 ns        15066 ns        45711 elements per second=137.133M
stdlib/garbage                     12560 ns        12556 ns        53934 elements per second=161.802M
```

## Performance for sin(double)
This is on a 3 GHz 2018 AMD desktop cpu
```
----------------------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------
nooplib/overhead_measure_tiny        463 ns          463 ns      1309430 elements per second=3.92829G
vectorlib/tiny                     10289 ns        10288 ns        68014 elements per second=204.042M
vectorlib/medium                   10375 ns        10372 ns        67713 elements per second=203.139M
vectorlib/increasing               10302 ns        10297 ns        66890 elements per second=200.67M
vectorlib/garbage                  17523 ns        17518 ns        39850 elements per second=119.55M
stdlib/tiny                        19023 ns        19004 ns        36307 elements per second=108.921M
stdlib/medium                      39093 ns        39088 ns        17900 elements per second=53.7M
stdlib/increasing                  30784 ns        30781 ns        22700 elements per second=68.1M
stdlib/garbage                     89507 ns        89427 ns         7728 elements per second=23.184M
```
