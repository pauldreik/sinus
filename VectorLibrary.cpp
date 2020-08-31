#include "vectorclass.h"
#include "vectormath_trig.h"

#include "VectorLibrary.hpp"

#include <cassert>

void
VectorLibrary::inplace_sin(float* p, std::size_t N)
{
  // round down to multiple of 16
  std::size_t N16 = (N / 16) * 16;

  std::size_t i = 0;
  for (i = 0; i < N16; i += 16) {
    sin(Vec8f().load(p + i + 0)).store(p + i + 0);
    sin(Vec8f().load(p + i + 8)).store(p + i + 8);
  }
  if (N16 - i >= 8) {
    sin(Vec8f().load(p + i + 0)).store(p + i + 0);
    i += 8;
  }
  const auto Nleft = static_cast<int>(N - i);

  sin(Vec8f().load_partial(Nleft, p + i)).store_partial(Nleft, p + i);
}

void
VectorLibrary::inplace_sin(double* p, std::size_t N)
{
  // round down to multiple of 8
  std::size_t N8 = (N / 8) * 8;

  std::size_t i = 0;
  for (i = 0; i < N8; i += 8) {
    sin(Vec4d().load(p + i + 0)).store(p + i + 0);
    sin(Vec4d().load(p + i + 4)).store(p + i + 4);
  }
  if (N8 - i >= 4) {
    sin(Vec4d().load(p + i + 0)).store(p + i + 0);
    i += 4;
  }
  const auto Nleft = static_cast<int>(N - i);

  sin(Vec4d().load_partial(Nleft, p + i)).store_partial(Nleft, p + i);
}
