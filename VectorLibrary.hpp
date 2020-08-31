#pragma once

#include "ImplementationBase.hpp"
#include <cstddef>

struct VectorLibrary : ImplementationBase
{
  // vector summation has speed 10 cycles/element with avx2
  // 16 cycles/element with garbage input (mostly huge)
  static double evaluate_sin(const double* p, int n);

  static float evaluate_sin(const float* p, int n);

  static void inplace_sin(float* p, std::size_t N);
};
