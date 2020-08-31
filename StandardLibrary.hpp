#pragma once

#include "ImplementationBase.hpp"

#include <cstddef>

struct StandardLibrary : ImplementationBase {
    static void inplace_sin(float* p, std::size_t N);
    static void inplace_sin(double* p, std::size_t N);
};
