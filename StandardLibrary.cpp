#include "StandardLibrary.hpp"

#include <cmath>

namespace {
template<typename T>
void
generic_inplace_sin(T* p, std::size_t N)
{
  for (std::size_t i = 0; i < N; ++i) {
    p[i] = std::sin(p[i]);
  }
}
}

void
StandardLibrary::inplace_sin(float* p, std::size_t N)
{
  generic_inplace_sin(p, N);
}
void
StandardLibrary::inplace_sin(double* p, std::size_t N)
{
  generic_inplace_sin(p, N);
}
