#include "vectorclass.h"
#include "vectormath_trig.h"

#include "VectorLibrary.hpp"

#include <cassert>

double VectorLibrary::evaluate_sin(const double *p, int n) {
    // based on the vectorclass summation example in the documentation

    assert(n>0);

    int n2 = n & (-8); //  round  down n to  multiple  of 8
    Vec4d sum1(0.0), sum2(0.0);
    int i;
    for (i = 0; i < n2; i += 8) {
        sum1 += sin(Vec4d().load(p + i));
        sum2 += sin(Vec4d().load(p + i + 4)); // 4 more  numbers
    }
    if (n - i >= 4) { // add 4 more  numbers
        sum1 += sin(Vec4d().load(p + i));
        i += 4;
    } // add  any  remaining  numbers (the top is zero, sin(zero)=zero
    sum2 += sin(Vec4d().load_partial(n - i, p + i));
    return horizontal_add(sum1 + sum2);
}
