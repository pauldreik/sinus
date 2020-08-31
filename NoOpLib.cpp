#include "NoOpLib.hpp"

void
NoOpLib::inplace_sin(float* p, std::size_t N)
{
  // do nothing - this measures benchmark overhead
}

void
NoOpLib::inplace_sin(double* p, std::size_t N)
{
  // do nothing - this measures benchmark overhead
}
