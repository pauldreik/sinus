#include "vectorclass.h"
#include "vectormath_trig.h"

#include "VectorLibrary.hpp"

#include <cassert>


namespace paulsin {


// *************************************************************
//             sin/cos template, double precision
// *************************************************************
// Template parameters:
// VTYPE:  f.p. vector type
// SC:     1 = sin, 2 = cos, 3 = sincos
// Paramterers:
// xx = input x (radians)
// cosret = return pointer (only if SC = 3)
//template<typename VTYPE, int SC>
static inline Vec4d sincos_d( Vec4d const xorig) {

    using VTYPE=Vec4d;
    constexpr int SC=1;
    VTYPE* cosret=nullptr;

    // define constants
    const double P0sin = -1.66666666666666307295E-1;
    const double P1sin = 8.33333333332211858878E-3;
    const double P2sin = -1.98412698295895385996E-4;
    const double P3sin = 2.75573136213857245213E-6;
    const double P4sin = -2.50507477628578072866E-8;
    const double P5sin = 1.58962301576546568060E-10;

    const double P0cos = 4.16666666666665929218E-2;
    const double P1cos = -1.38888888888730564116E-3;
    const double P2cos = 2.48015872888517045348E-5;
    const double P3cos = -2.75573141792967388112E-7;
    const double P4cos = 2.08757008419747316778E-9;
    const double P5cos = -1.13585365213876817300E-11;

    const double DP1 = 7.853981554508209228515625E-1 * 2.;
    const double DP2 = 7.94662735614792836714E-9 * 2.;
    const double DP3 = 3.06161699786838294307E-17 * 2.;
    /*
    const double DP1sc = 7.85398125648498535156E-1;
    const double DP2sc = 3.77489470793079817668E-8;
    const double DP3sc = 2.69515142907905952645E-15;
    */
    typedef decltype(roundi(xorig)) ITYPE;          // integer vector type
    typedef decltype(nan_code(xorig)) UITYPE;       // unsigned integer vector type
    typedef decltype(xorig < xorig) BVTYPE;            // boolean vector type

    VTYPE  xabs, x, yscaled, x2, s, c, sin1, cos1;       // data vectors
    ITYPE  quadrant, qq, signsin, signcos;              // integer vectors, 64 bit

    BVTYPE swap, overflow;                       // boolean vectors


    //findsector
    constexpr unsigned long Nsectors=8; // must be power of two so we can do modulo efficiently
    const VTYPE yscaledNsector = round(xorig * (Nsectors / (2*VM_PI)));        // quadrant, as float
//    auto debug_yscaledNsector=yscaledNsector[0];
    const ITYPE quadrantNsector = roundi(yscaledNsector);                               // quadrant, as integer
//    auto debug_quadrantNsector=quadrantNsector[0];


    // Reduce by extended precision modular arithmetic
    const VTYPE xNsector = nmul_add(yscaledNsector,
                              DP3*4/Nsectors,
                              nmul_add(yscaledNsector,
                                       DP2*4/Nsectors,
                                       nmul_add(yscaledNsector,
                                                DP1*4/Nsectors,
                                                xabs)));    // x = ((xa - y * DP1) - y * DP2) - y * DP3;
//    auto debug_xNsector=xNsector[0];

    // Expansion of sin and cos, valid for -pi/4 <= x <= pi/4
    VTYPE x2Nsector = xNsector*xNsector;
//    auto debug_x2Nsector=x2Nsector[0];

//    VTYPE sNsector = polynomial_5(x2Nsector, P0sin, P1sin, P2sin, P3sin, P4sin, P5sin);
//    VTYPE cNsector = polynomial_5(x2Nsector, P0cos, P1cos, P2cos, P3cos, P4cos, P5cos);
    VTYPE sNsector = polynomial_2(x2Nsector, P0sin, P1sin, P2sin);//, P3sin, P4sin, P5sin);
    VTYPE cNsector = polynomial_2(x2Nsector, P0cos, P1cos, P2cos);//, P3cos, P4cos, P5cos);
    sNsector=mul_add(xNsector * x2Nsector, sNsector, xNsector);
    cNsector = mul_add(x2Nsector * x2Nsector, cNsector, nmul_add(x2Nsector, 0.5, 1.0));
//    auto debug_sNsector=sNsector[0];
//    auto debug_cNsector=cNsector[0];
    // rotate into place with the rotation matrix
    //  [ rC  -rS] [cNsector]
    //  [ rS rC] [sNsector]
    // disp(sprintf('%#.3g, ',cos([0:15]/16*2*pi)))
    /*
    static_assert(Nsectors==16);
    const double Ctable[Nsectors]={  1.00, 0.924, 0.707, 0.383, 6.12e-17, -0.383, -0.707, -0.924, -1.00, -0.924, -0.707, -0.383, -1.84e-16, 0.383, 0.707, 0.924};
    const double Stable[Nsectors]={0.00, 0.383, 0.707, 0.924, 1.00, 0.924, 0.707, 0.383, 1.22e-16, -0.383, -0.707, -0.924, -1.00, -0.924, -0.707, -0.383};
    */
    static_assert(Nsectors==8);
    const double Ctable[Nsectors]={ 1.00, 0.707, 6.12e-17, -0.707, -1.00, -0.707, -1.84e-16, 0.707, };
    const double Stable[Nsectors]={0.00, 0.707, 1.00, 0.707, 1.22e-16, -0.707, -1.00, -0.707, };

//    auto debug_index=(quadrantNsector & (Nsectors-1))[0];
    const auto index=quadrantNsector & (Nsectors-1);
    VTYPE rC=lookup<Nsectors>(index,Ctable);
    VTYPE rS=lookup<Nsectors>(index,Stable);
//    auto debug_rC=rC[0];

    auto rotated_cos=mul_sub(rC,cNsector,rS*sNsector); // rC*cNsector-rS*sNsector
    auto rotated_sin=mul_add(rS,cNsector,rC*sNsector);

//    auto debug_rot_cos=rotated_cos[0];
//    auto debug_rot_sin=rotated_sin[0];


    // check for overflow
    overflow = BVTYPE(UITYPE(quadrantNsector) > 0x80000000000000);  // q big if overflow
//    auto debug_overflow=overflow[0];

    overflow &= is_finite(xorig);
//    debug_overflow=overflow[0];

    rotated_sin = select(overflow, 0.0, rotated_sin);
    rotated_cos = select(overflow, 1.0, rotated_cos);

    if constexpr (SC == 3) {  // calculate both. cos returned through pointer
        *cosret = rotated_cos;
    }
    if constexpr ((SC & 1) != 0)
            return rotated_sin;
    else
    return rotated_cos;
}

}

void
PaulVectorLibrary::inplace_sin(float* p, std::size_t N)
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
PaulVectorLibrary::inplace_sin(double* p, std::size_t N)
{
  // round down to multiple of 8
  std::size_t N8 = (N / 8) * 8;

  std::size_t i = 0;
  for (i = 0; i < N8; i += 8) {
    paulsin::sincos_d(Vec4d().load(p + i + 0)).store(p + i + 0);
    paulsin::sincos_d(Vec4d().load(p + i + 4)).store(p + i + 4);
  }
  if (N8 - i >= 4) {
    paulsin::sincos_d(Vec4d().load(p + i + 0)).store(p + i + 0);
    i += 4;
  }
  const auto Nleft = static_cast<int>(N - i);

  paulsin::sincos_d(Vec4d().load_partial(Nleft, p + i)).store_partial(Nleft, p + i);
}


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
