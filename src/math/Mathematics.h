
#pragma once

#ifndef MATHEMATICS_H
#define MATHEMATICS_H

#include <math.h>

#include "../System.h"

#ifdef ___WINDOWS_NT
#pragma warning(push)
#pragma warning(disable : 4310)
#endif

////////////////////////////
// Mathematical Constants //
////////////////////////////

/// _INF represents negative infinity


namespace Burns
{

namespace FLOAT32
{
   /// Constants
   static const float _INF    = cast<float>(0xff800000);
   static const float INF     = cast<float>(0x7f800000);
   static const float MAXV    = cast<float>(0x7f7fffff);
   static const float EPSILON = cast<float>(0x34000000);
   static const float NORM    = cast<float>(0x00800000);
   static const float DENORM  = cast<float>(0x00000001);
   static const float E       = 2.7182818284590452353602875f;
   static const float PI      = 3.1415926535897932384626433832795f;
   static const float RCP_PI  = 0.31830988618379067154f;
   static const float PHI     = 1.6180339887498948482045868343656f;
   static const float SQRT_2  = 1.4142135623730950488016887242097f;
};

namespace FLOAT64
{
   /// Constants
   static const double _INF    = cast<double>(0xfff0000000000000ULL);
   static const double INF     = cast<double>(0x7ff0000000000000ULL);
   static const double MAXV    = cast<double>(0x7fefffffffffffffULL);
   static const double EPSILON = cast<double>(0x3cb0000000000000ULL);
   static const double NORM    = cast<double>(0x0010000000000000ULL);
   static const double DENORM  = cast<double>(0x0000000000000001ULL);
   static const double E       = 2.7182818284590452353602875;
   static const double PI      = 3.1415926535897932384626433832795;
   static const double RCP_PI  = 0.31830988618379067154;
   static const double PHI     = 1.6180339887498948482045868343656;
   static const double SQRT_2  = 1.4142135623730950488016887242097;
};

///////////////////
// Miscellaneous //
///////////////////

/// Compile-time integer Log-base-2 calculation. N must be greater than zero.
template <int N> struct ILOG2
{
private:

   template<int WIDTH, int DEPTH> struct RLOG2
   { enum { value = RLOG2<WIDTH/2, DEPTH+1>::value }; };
   template<int DEPTH> struct RLOG2<1, DEPTH> { enum { value = DEPTH }; };

public:
   static const int value = RLOG2<N, 0>::value;
};

/// Compile-time integer power calculation
template <int N, int E> struct IPOW
{
private:
   template <int M, int F> struct RPOW
   { enum { value = M * RPOW<M, F-1>::value }; };
   template <int M> struct RPOW<M, 1> { enum { value = M }; };
   template <int M> struct RPOW<M, 0> { enum { value = 1 }; };

public:
   static const int value = RPOW<N, E>::value;
};


/// Rounds the integer argument i to the next largest multiple of N,
/// where N must be a power of two
template <int N> FINLINE int nextPowerOf2Multiple(int i)
{ return ((i + N-1) >> ILOG2<N>::value) << ILOG2<N>::value; }

/// Integer base-2 logarithm functions
FINLINE uint32 ilog2(uint32 x)
{
   int r = 0;
   while ((x >> r) != 0) r++;
   return r-1; // returns -1 for x == 0, floor(log2(x)) otherwise
}

FINLINE uint64 ilog2(uint64 x)
{
   int r = 0;
   while ((x >> r) != 0) r++;
   return r-1; // returns -1 for x == 0, floor(log2(x)) otherwise
}



/// Factorization: Factors an integer into it's two most nearly equal integer
/// factors. For n that is prime, the results are 1 and n. If n is a pefect
/// square, then both results are sqrt(n). This is mostly to construct the
/// dimensions of the most-nearly-square image to represent some one
/// dimensional array.
///
/*FINLINE Burns::Pair<int> factorNearlyEqual(int i)
{
   float s = sqrtf((float)i);
   int low = (int)::floorf(s), high = i / low;
   while (low * high != i) { low--; high = i / low; }

   ASSERT(low >= 1 && high <= i && high >=low);
   return Burns::Pair<int>(low, high);
}*/

/// Returns true if the argument is a power of two. Use only on integral
/// types (int8, uint16, etc)
template <typename T> FINLINE bool isPowerOfTwo(T n)
{
   unsigned count;
   for (count = 0; n != 0; count++) n &= n - 1;
   return count == 1;
}


/// Given a binary integer value x, the next largest power of 2 can be
/// computed by a SWAR algorithm that recursively "folds" the upper bits into
/// the lower bits. This process yields a bit vector with the same most
/// significant 1 as x, but all 1's below it. Adding 1 to that value yields
/// the next largest power of 2.
///
template <class T> FINLINE T nextLargestPowerOfTwo(T i)
{

   i -= T(1);      // Sutract 1 so that powers of two produce themselves
   i |= (i >> 1);
   i |= (i >> 2);
   i |= (i >> 4);
   i |= (i >> 8);
   i |= (i >> 16);
   return i + T(1);
}


/// Linear interpolation between two coordinates. Parameter 't' is presumed
/// to be between 0.0 and 1.0
template <class T> FINLINE T lerp(T coord0, T coord1, float t)
{ return (1.0f - t) * coord0 + t*coord1; }



/*template <class T> FINLINE T isEven(const T& n)
{ STATIC_ASSERT(TypeInfo<T>::Integral); return !(n & 1); }
template <class T> FINLINE T isOdd (const T& n)
{ STATIC_ASSERT(TypeInfo<T>::Integral); return n & 1; }*/


/////////////////
// Mathematics //
/////////////////

/// Mathematics class (functioning as a static namespace) for defining common
/// mathematical constructs for specialized math types. Specialize this class
/// for math types which define these tokens. If a type does not define a
/// particular element of this static structure...
/// The default implementation assumes conversions to and from double-precision
/// floating point values and uses the standard library calls where applicable
///
template <class T> struct MATHEMATICS
{
   static FINLINE T zero()           { return (T) 0.0; }
   static FINLINE T one()            { return (T) 1.0; }
   static FINLINE T minusOne()       { return (T) -1.0; }
   static FINLINE T inf()            { return (T) FLOAT64::INF; }
   static FINLINE T _inf()           { return (T) FLOAT64::_INF; }
   static FINLINE T maxv()           { return (T) FLOAT64::MAXV; }
   static FINLINE T e()              { return (T) FLOAT64::E; }
   static FINLINE T pi()             { return (T) FLOAT64::PI; }
   static FINLINE T rcpPi()          { return (T) FLOAT64::RCP_PI; }
   static FINLINE T phi()            { return (T) FLOAT64::PHI; }
   static FINLINE T sqrt2()          { return (T) FLOAT64::SQRT_2; }

   static FINLINE T cbrt(T x)        { return (T) ::cbrt((double)x); }
   static FINLINE T sqrt(T x)        { return (T) ::sqrt((double)x); }
   static FINLINE T rcpSqrt(T x)     { return (T) (1.0 / ::sqrt((double)x)); }
   static FINLINE T rcp(T x)         { return (T) (1.0 / (double)x); }
   static FINLINE T cos(T x)         { return (T) ::cos((double)x); }
   static FINLINE T sin(T x)         { return (T) ::sin((double)x); }
   static FINLINE T tan(T x)         { return (T) ::tan((double)x); }
   static FINLINE T pow(T x, T y)    { return (T) ::pow((double)x, (double)y); }
   static FINLINE T exp(T x)         { return (T) ::exp((double)x); }
   static FINLINE T log(T x)         { return (T) ::log((double)x); }
   static FINLINE T log2(T x)        { return (T) ::log2((double)x); }
   static FINLINE T log10(T x)       { return (T) ::log10((double)x); }
   static FINLINE T abs(T x)         { return (T) ::fabs((double)x); }
   static FINLINE T acos(T x)        { return (T) ::acos((double)x); }
   static FINLINE T asin(T x)        { return (T) ::asin((double)x); }
   static FINLINE T atan2(T x, T y)  { return (T) ::atan2((double)x, (double)y); }
   static FINLINE T floor(T x)       { return (T) ::floor((double)x); }
   static FINLINE T ceil(T x)        { return (T) ::ceil((double)x); }
   static FINLINE T sign(T x)        { return (T)(x > 0.0 ? 1.0 : (x < 0.0 ? -1.0 : 0.0)); }

   // Clamp from below by zero, above by the given value
   static FINLINE T clampz(T x, T u) { return (x < zero() ? 0.0f : (x > u ? u : x)); }

   static FINLINE T min(T x, T y)          { return (x < y ? x : y); }
   static FINLINE T max(T x, T y)          { return (x > y ? x : y); }
   static FINLINE T min3(T x, T y, T z)    { return min(x, min(y, z)); }
   static FINLINE T max3(T x, T y, T z)    { return max(x, max(y, z)); }
};


/// Specialization for uint8s
template <> struct MATHEMATICS<uint8>
{
   static FINLINE uint8 zero()          { return 0u; }
   static FINLINE uint8 one()           { return 1u; }
   static FINLINE uint8 inf()           { return 0xffu; }
   static FINLINE uint8 _inf()          { return 0x00u; }

   static FINLINE uint8 clampz(uint8 x, uint8 u)
   { return (x > u ? u : x); }

   static FINLINE uint8 log2(uint8 x)                   { return (uint8)ilog2((uint32)x); }

   //static FINLINE uint8 sign(uint8 x)                   { return 0u; }
   static FINLINE uint8 min(uint8 x, uint8 y)           { return (x < y ? x : y); }
   static FINLINE uint8 max(uint8 x, uint8 y)           { return (x > y ? x : y); }
   static FINLINE uint8 min3(uint8 x, uint8 y, uint8 z) { return min(x, min(y, z)); }
   static FINLINE uint8 max3(uint8 x, uint8 y, uint8 z) { return max(x, max(y, z)); }
};

template <> struct MATHEMATICS<unorm8>
{
   static FINLINE unorm8 zero()         { return unorm8(0u); }
   static FINLINE unorm8 one()          { return unorm8(255u); }
};

/// Specialization for uint16s
template <> struct MATHEMATICS<uint16>
{
   static FINLINE uint16 zero()          { return 0u; }
   static FINLINE uint16 one()           { return 1u; }
   static FINLINE uint16 inf()           { return 0xffffu; }
   static FINLINE uint16 _inf()          { return 0x0000u; }

   static FINLINE uint16 clampz(uint16 x, uint16 u)
   { return (x > u ? u : x); }

   static FINLINE uint16 log2(uint16 x)                      { return (uint16)ilog2((uint32)x); }

   //static FINLINE uint16 sign(uint16 x)                     { return 0u; }
   static FINLINE uint16 min(uint16 x, uint16 y)            { return (x < y ? x : y); }
   static FINLINE uint16 max(uint16 x, uint16 y)            { return (x > y ? x : y); }
   static FINLINE uint16 min3(uint16 x, uint16 y, uint16 z) { return min(x, min(y, z)); }
   static FINLINE uint16 max3(uint16 x, uint16 y, uint16 z) { return max(x, max(y, z)); }

   static FINLINE uint16 maxv() { return 0xffffu; }
};

template <> struct MATHEMATICS<unorm16>
{
   static FINLINE unorm16 zero()         { return unorm16(0u); }
   static FINLINE unorm16 one()          { return unorm16(65535u); }
};

/// Specialization for uint32s
template <> struct MATHEMATICS<uint32>
{
   static FINLINE uint32 zero()          { return 0u; }
   static FINLINE uint32 one()           { return 1u; }
   static FINLINE uint32 inf()           { return 0xffffffffu; }
   static FINLINE uint32 _inf()          { return 0x00000000u; }

   static FINLINE uint32 clampz(uint32 x, uint32 u)
   { return (x > u ? u : x); }

   static FINLINE uint32 log2(uint32 x)                      { return (uint32)ilog2(x); }

   //static FINLINE uint16 sign(uint32 x)                     { return 0u; }
   static FINLINE uint32 min(uint32 x, uint32 y)            { return (x < y ? x : y); }
   static FINLINE uint32 max(uint32 x, uint32 y)            { return (x > y ? x : y); }
   static FINLINE uint32 min3(uint32 x, uint32 y, uint32 z) { return min(x, min(y, z)); }
   static FINLINE uint32 max3(uint32 x, uint32 y, uint32 z) { return max(x, max(y, z)); }

   static FINLINE uint32 maxv() { return 0xffffffffu; }
};

/// Specialization for uint64s
template <> struct MATHEMATICS<uint64>
{
   static FINLINE uint64 zero()          { return 0ULL; }
   static FINLINE uint64 one()           { return 1ULL; }
   static FINLINE uint64 inf()           { return 0xffffffffffffffffULL; }
   static FINLINE uint64 _inf()          { return 0x0000000000000000ULL; }

   static FINLINE uint64 clampz(uint64 x, uint64 u)
   { return (x > u ? u : x); }

   static FINLINE uint64 log2(uint64 x)                      { return ilog2(x); }

   //static FINLINE uint16 sign(uint64 x)                     { return 0ULL; }
   static FINLINE uint64 min(uint64 x, uint64 y)            { return (x < y ? x : y); }
   static FINLINE uint64 max(uint64 x, uint64 y)            { return (x > y ? x : y); }
   static FINLINE uint64 min3(uint64 x, uint64 y, uint64 z) { return min(x, min(y, z)); }
   static FINLINE uint64 max3(uint64 x, uint64 y, uint64 z) { return max(x, max(y, z)); }

   static FINLINE uint64 maxv() { return 0xffffffffffffffffULL; }
};

/// Specialization for int8s
template <> struct MATHEMATICS<int8>
{
   static FINLINE int8 zero()          { return 0; }
   static FINLINE int8 one()           { return 1; }
   static FINLINE int8 minusOne()      { return -1; }
   static FINLINE int8 inf()           { return int8(0x7f); }
   static FINLINE int8 _inf()          { return int8(0x81); }

   static FINLINE int8 clampz(int8 x, int8 u)
   { return (x < 0 ? 0 : (x > u ? u : x)); }
   static FINLINE int8 sign(int8 x) { return (x < 0 ? -1 : (x > 0 ? 1 : 0)); }

   //static FINLINE int8 sign(int8 x) { return (uint8(x) >> 7); } // right logical shift
   static FINLINE int8 abs(int8 x)                     { return (int8)::abs(x); }
   static FINLINE int8 min(int8 x, int8 y)             { return (x < y ? x : y); }
   static FINLINE int8 max(int8 x, int8 y)             { return (x > y ? x : y); }
   static FINLINE int8 min3(int8 x, int8 y, int8 z)    { return min(x, min(y, z)); }
   static FINLINE int8 max3(int8 x, int8 y, int8 z)    { return max(x, max(y, z)); }

   static FINLINE int8 maxv() { return 0x7f; }
};

/// Specialization for int8s
template <> struct MATHEMATICS<int16>
{
   static FINLINE int16 zero()          { return 0; }
   static FINLINE int16 one()           { return 1; }
   static FINLINE int16 minusOne()      { return -1; }
   static FINLINE int16 inf()           { return (int16)0x7fff; }
   static FINLINE int16 _inf()          { return (int16)0x8001; }

   static FINLINE int16 clampz(int16 x, int16 u)
   { return (x < 0 ? 0 : (x > u ? u : x)); }
   static FINLINE int16 sign(int16 x) { return (x < 0 ? -1 : (x > 0 ? 1 : 0)); }

   //static FINLINE int8 sign(int16 x) { return (uint16(x) >> 15); } // right logical shift
   static FINLINE int16 abs(int16 x)                    { return (int16)::abs(x); }
   static FINLINE int16 min(int16 x, int16 y)           { return (x < y ? x : y); }
   static FINLINE int16 max(int16 x, int16 y)           { return (x > y ? x : y); }
   static FINLINE int16 min3(int16 x, int16 y, int16 z) { return min(x, min(y, z)); }
   static FINLINE int16 max3(int16 x, int16 y, int16 z) { return max(x, max(y, z)); }

   static FINLINE int16 maxv() { return 0x7fff; }
};

/// Specialization for int8s
template <> struct MATHEMATICS<int32>
{
   static FINLINE int32 zero()          { return 0; }
   static FINLINE int32 one()           { return 1; }
   static FINLINE int32 minusOne()      { return -1; }
   static FINLINE int32 inf()           { return 0x7fffffff; }
   static FINLINE int32 _inf()          { return 0x80000001; }

   static FINLINE int32 clampz(int32 x, int32 u)
   { return (x < zero() ? zero() : (x > u ? u : x)); }
   static FINLINE int32 sign(int32 x) { return (x < 0 ? -1 : (x > 0 ? 1 : 0)); }

   //static FINLINE int8 sign(int32 x) { return (uint32(x) >> 31); } // right logical shift
   static FINLINE int32 abs(int32 x) { return ::abs(x); }
   static FINLINE int32 min(int32 x, int32 y)           { return (x < y ? x : y); }
   static FINLINE int32 max(int32 x, int32 y)           { return (x > y ? x : y); }
   static FINLINE int32 min3(int32 x, int32 y, int32 z) { return min(x, min(y, z)); }
   static FINLINE int32 max3(int32 x, int32 y, int32 z) { return max(x, max(y, z)); }

   static FINLINE int32 maxv() { return 0x7fffffff; }
};

/// Specialization for int64s
template <> struct MATHEMATICS<int64>
{
   static FINLINE int64 zero()          { return 0LL; }
   static FINLINE int64 one()           { return 1LL; }
   static FINLINE int64 minusOne()      { return -1LL; }
   static FINLINE int64 inf()           { return 0x7fffffffffffffffLL; }
   static FINLINE int64 _inf()          { return 0x8000000000000001LL; }

   static FINLINE int64 clampz(int64 x, int64 u)
   { return (x < zero() ? zero() : (x > u ? u : x)); }
   static FINLINE int64 sign(int64 x) { return (x < 0 ? -1 : (x > 0 ? 1 : 0)); }

   //static FINLINE int8 sign(int64 x) { return (uint64(x) >> 63); } // right logical shift
   static FINLINE int64 abs(int64 x) { return _abs64(x); }
   static FINLINE int64 min(int64 x, int64 y)           { return (x < y ? x : y); }
   static FINLINE int64 max(int64 x, int64 y)           { return (x > y ? x : y); }
   static FINLINE int64 min3(int64 x, int64 y, int64 z) { return min(x, min(y, z)); }
   static FINLINE int64 max3(int64 x, int64 y, int64 z) { return max(x, max(y, z)); }

   static FINLINE int64 maxv() { return 0x7fffffffffffffffLL; }
};



// Global namespace wrappers for the Math class namespace...
template <class T> FINLINE T ZERO()           { return MATHEMATICS<T>::zero(); }
template <class T> FINLINE T ONE()            { return MATHEMATICS<T>::one(); }
template <class T> FINLINE T MINUS_ONE()      { return MATHEMATICS<T>::minusOne(); }
template <class T> FINLINE T INF()            { return MATHEMATICS<T>::inf(); }
template <class T> FINLINE T _INF()           { return MATHEMATICS<T>::_inf(); }
template <class T> FINLINE T MAXV()           { return MATHEMATICS<T>::maxv(); }
template <class T> FINLINE T E()              { return MATHEMATICS<T>::e(); }
template <class T> FINLINE T PI()             { return MATHEMATICS<T>::pi(); }
template <class T> FINLINE T RCP_PI()         { return MATHEMATICS<T>::rcpPi(); }
template <class T> FINLINE T PHI()            { return MATHEMATICS<T>::phi(); }
template <class T> FINLINE T SQRT2()          { return MATHEMATICS<T>::sqrt2(); }

template <class T> FINLINE T CBRT(T x)        { return MATHEMATICS<T>::cbrt(x); }
template <class T> FINLINE T SQRT(T x)        { return MATHEMATICS<T>::sqrt(x); }
template <class T> FINLINE T RCP_SQRT(T x)    { return MATHEMATICS<T>::rcpSqrt(x); }
template <class T> FINLINE T RCP(T x)         { return MATHEMATICS<T>::rcp(x); }
template <class T> FINLINE T COS (T x)        { return MATHEMATICS<T>::cos(x); }
template <class T> FINLINE T SIN (T x)        { return MATHEMATICS<T>::sin(x); }
template <class T> FINLINE T TAN (T x)        { return MATHEMATICS<T>::tan(x); }
template <class T> FINLINE T POW (T x, T y)   { return MATHEMATICS<T>::pow(x, y); }
template <class T> FINLINE T EXP (T x)        { return MATHEMATICS<T>::exp(x); }
template <class T> FINLINE T LOG (T x)        { return MATHEMATICS<T>::log(x); }
template <class T> FINLINE T LOG2 (T x)       { return MATHEMATICS<T>::log2(x); }

/// Not sure what's going on here, but iOS build gives the error :
///    "Expected unqualified-id before '{' token".
/// Renaming "ABS" to something else fixes this...
#if defined(___IOS)
template <class T> FINLINE T ABS_ (T x)         { return MATHEMATICS<T>::abs(x); }
#else
template <class T> FINLINE T ABS (T x)         { return MATHEMATICS<T>::abs(x); }
#endif
template <class T> FINLINE T LOG10 (T x)       { return MATHEMATICS<T>::log10(x); }
template <class T> FINLINE T ACOS (T x)        { return MATHEMATICS<T>::acos(x); }
template <class T> FINLINE T ASIN (T x)        { return MATHEMATICS<T>::asin(x); }
template <class T> FINLINE T ATAN2 (T x, T y)  { return MATHEMATICS<T>::atan2(x,y); }
template <class T> FINLINE T FLOOR (T x)       { return MATHEMATICS<T>::floor(x); }
template <class T> FINLINE T CEILING (T x)     { return MATHEMATICS<T>::ceil(x); }
template <class T> FINLINE T CLAMPZ (T x, T u) { return MATHEMATICS<T>::clampz(x, u); }
template <class T> FINLINE T SIGN (T x)        { return MATHEMATICS<T>::sign(x); }


#ifdef MIN
#undef MIN
#endif
#ifdef MAX
#undef MAX
#endif
template <class T> FINLINE T MIN(T x, T y)       { return MATHEMATICS<T>::min(x, y); }
template <class T> FINLINE T MAX(T x, T y)       { return MATHEMATICS<T>::max(x, y); }
template <class T> FINLINE T MIN3(T x, T y, T z) { return MATHEMATICS<T>::min3(x, y, z); }
template <class T> FINLINE T MAX3(T x, T y, T z) { return MATHEMATICS<T>::max3(x, y, z); }



/// Conversion between degrees and radians. Defined for float and double
template <class T> FINLINE T DEG_TO_RAD(T x)
{ return (x) * (PI<T>()   / T(180.0)); }
template <class T> FINLINE T RAD_TO_DEG(T x)
{ return (x) * 180.0 * RCP_PI<double>(); }
template <> FINLINE float DEG_TO_RAD(float x)
{ return (x) * (PI<float>()   / 180.0f); }
template <> FINLINE float RAD_TO_DEG(float x)
{ return (x) * 180.0f * RCP_PI<float>(); }



/// A class which generates a random prime 32-bit number upon request. The
/// interface consists of one static method which accepts an integer input
/// which is incremented until a prime is found.
class Prime
{
 private:

   static FINLINE uint32 isqrt(uint32 i)
   {
      uint32 r = 0, rnew = 1, rold = r;
      do
      {
         rold = r;           r = rnew;
         rnew = (r + (i/r)); rnew >>= 1;
      } while (rold != rnew);

      return rnew;
   }

public:

   /// Returns the smallest prime number greater-than or equal to \param p
   static FINLINE uint32 nextPrime(uint32 p)
   {
      while (!isPrime(p)) ++p;
      return p;
   }

   /// Returns true if the integer i is prime, false otherwise
   static FINLINE bool isPrime(uint32 i)
   {
      uint32 si = isqrt(i);
      for (uint32 j = 2; j <= si; j++) if (i % j == 0) return false;
      return true;
   }
};

}; // namespace Burns



// Now include all the mathematical types in the library
#include "Vector.h"
#include "Mask.h"

// SSE is enabled and included by default
#if defined(___SSE)
#include "SSE/SSE.h"    // Includes Vector4i, Vector4f, SSEMask, SSERegister
#endif // ___SSE

#if defined(___AVX)
#include "AVX/AVX.h"
#endif


namespace Burns
{

/// Specialization for single-precision floats. This used to come earlier in
/// the file here, but was moved after we included SSE so that we
/// could use SSE intrinsics for some of these special functions
template <> struct MATHEMATICS<float>
{
   static FINLINE float zero()          { return 0.0f; }
   static FINLINE float one()           { return 1.0f; }
   static FINLINE float minusOne()      { return -1.0f; }
   static FINLINE float inf()           { return FLOAT32::INF; }
   static FINLINE float _inf()          { return FLOAT32::_INF; }
   static FINLINE float maxv()          { return FLOAT32::MAXV; }
   static FINLINE float e()             { return FLOAT32::E; }
   static FINLINE float pi()            { return FLOAT32::PI; }
   static FINLINE float rcpPi()         { return FLOAT32::RCP_PI; }
   static FINLINE float phi()           { return FLOAT32::PHI; }
   static FINLINE float sqrt2()         { return FLOAT32::SQRT_2; }

   static FINLINE float cbrt(float x)            { return ::cbrtf(x); }
   static FINLINE float sqrt(float x)            { return ::sqrtf(x); }
   static FINLINE float rcpSqrt(float x)         { return (1.0f / ::sqrtf(x)); }
   static FINLINE float rcp(float x)             { return (1.0f / x); }
   static FINLINE float cos(float x)             { return ::cosf(x); }
   static FINLINE float sin(float x)             { return ::sinf(x); }
   static FINLINE float tan(float x)             { return ::tanf(x); }
   static FINLINE float pow(float x, float y)    { return ::powf(x, y); }
   static FINLINE float exp(float x)             { return ::expf(x); }
   static FINLINE float log(float x)             { return ::logf(x); }
   static FINLINE float log2(float x)            { return ::log2f(x); }
   static FINLINE float log10(float x)           { return ::log10f(x); }
   static FINLINE float abs(float x)             { return ::fabsf(x); }
   static FINLINE float acos(float x)            { return ::acosf(x); }
   static FINLINE float asin(float x)            { return ::asinf(x); }
   static FINLINE float atan2(float x, float y)  { return ::atan2f(x, y); }

#if defined(___SSE4)
   static FINLINE float floor(float x)           { return SSERegister(x).fpFloor().fpExtract<0>(); }
   static FINLINE float ceil(float x)            { return SSERegister(x).fpCeiling().fpExtract<0>(); }
   static FINLINE float clampz(float x, float u) { return SSERegister(x).fpClamp(SSERegister::zeroes(), SSERegister(u)).fpExtract<0>(); }
#else
   static FINLINE float floor(float x)           { return ::floorf(x); }
   static FINLINE float ceil(float x)            { return ::ceilf(x); }
   static FINLINE float clampz(float x, float u) { return (x < 0.0f ? 0.0f : (x > u ? u : x)); }
#endif

   static FINLINE float sign(float x)            { return (x > 0.0f ? 1.0f : (x < 0.0f ? -1.0f : 0.0f)); }
   static FINLINE float min(float x, float y)           { return (x < y ? x : y); }
   static FINLINE float max(float x, float y)           { return (x > y ? x : y); }
   static FINLINE float min3(float x, float y, float z) { return min(x, min(y, z)); }
   static FINLINE float max3(float x, float y, float z) { return max(x, max(y, z)); }
};







typedef Mask<bool, 2> Mask2;
typedef Mask<bool, 3> Mask3;
typedef Mask<bool, 4> Mask4;
typedef Mask<bool, 8> Mask8;
typedef Mask<bool, 16> Mask16;
typedef Mask<bool, 64> Mask64;

typedef Mask<Mask<bool, 4>, 2>  Mask2x4;
typedef Mask<Mask<bool, 16>, 2> Mask2x16;
typedef Mask<Mask<bool, 64>, 2> Mask2x64;

typedef Mask<Mask<bool, 4>, 3> Mask3x4;
typedef Mask<Mask<bool, 16>, 3> Mask3x16;
typedef Mask<Mask<bool, 64>, 3> Mask3x64;


typedef Vector<float, 2>  Vector2f;
typedef Vector<float, 3>  Vector3f;
typedef Vector<float, 4>  Vector4f;
typedef Vector<float, 8>  Vector8f;
typedef Vector<float, 16> Vector16f;
typedef Vector<float, 64> Vector64f;

typedef Vector<int32, 2>  Vector2i;
typedef Vector<int32, 3>  Vector3i;
typedef Vector<int32, 4>  Vector4i;
typedef Vector<int32, 8>  Vector8i;
typedef Vector<int32, 16> Vector16i;
typedef Vector<int32, 64> Vector64i;

typedef Vector<int64, 2>  Vector2l;
typedef Vector<int64, 3>  Vector3l;
typedef Vector<int64, 4>  Vector4l;
typedef Vector<int64, 8>  Vector8l;
typedef Vector<int64, 16> Vector16l;
typedef Vector<int64, 64> Vector64l;


typedef Vector<Vector<float, 4>, 2>   Vector2x4f;
typedef Vector<Vector<float, 8>, 2>   Vector2x8f;
typedef Vector<Vector<float, 16>, 2>  Vector2x16f;
typedef Vector<Vector<float, 64>, 2>  Vector2x64f;

typedef Vector<Vector<int32, 4>, 2>   Vector2x4i;
typedef Vector<Vector<int32, 8>, 2>   Vector2x8i;
typedef Vector<Vector<int32, 16>, 2>  Vector2x16i;
typedef Vector<Vector<int32, 64>, 2>  Vector2x64i;


typedef Vector<Vector<float, 2>, 3>   Vector3x2f;
typedef Vector<Vector<float, 3>, 3>   Vector3x3f;
typedef Vector<Vector<float, 4>, 3>   Vector3x4f;
typedef Vector<Vector<float, 8>, 3>   Vector3x8f;
typedef Vector<Vector<float, 16>, 3>  Vector3x16f;
typedef Vector<Vector<float, 64>, 3>  Vector3x64f;

typedef Vector<Vector<int32, 2>, 3>   Vector3x2i;
typedef Vector<Vector<int32, 3>, 3>   Vector3x3i;
typedef Vector<Vector<int32, 4>, 3>   Vector3x4i;
typedef Vector<Vector<int32, 8>, 3>   Vector3x8i;
typedef Vector<Vector<int32, 16>, 3>  Vector3x16i;
typedef Vector<Vector<int32, 64>, 3>  Vector3x64i;

typedef Vector<Vector<float, 2>, 4>   Vector4x2f;
typedef Vector<Vector<float, 3>, 4>   Vector4x3f;
typedef Vector<Vector<float, 4>, 4>   Vector4x4f;
typedef Vector<Vector<float, 8>, 4>   Vector4x8f;
typedef Vector<Vector<float, 16>, 4>  Vector4x16f;
typedef Vector<Vector<float, 64>, 4>  Vector4x64f;

typedef Vector<Vector<int32, 2>, 4>   Vector4x2i;
typedef Vector<Vector<int32, 3>, 4>   Vector4x3i;
typedef Vector<Vector<int32, 4>, 4>   Vector4x4i;
typedef Vector<Vector<int32, 8>, 4>   Vector4x8i;
typedef Vector<Vector<int32, 16>, 4>  Vector4x16i;
typedef Vector<Vector<int32, 64>, 4>  Vector4x64i;

/// Here we define a "canonical" computational vector to be used whenever we
/// simply want to use the native machine vector for data-parallel performance
#ifdef ___LRBni
typedef Vector16f VectorNf;
typedef Vector16i VectorNi;
typedef Mask16    MaskN;
#define SIMD_WIDTH 16
#else
typedef Vector4f VectorNf;
typedef Vector4i VectorNi;
typedef Vector4l VectorNl;
typedef Mask4    MaskN;
#define SIMD_WIDTH 4
#endif

typedef Vector<VectorNf, 2> Vector2xNf;
typedef Vector<VectorNi, 2> Vector2xNi;
typedef Vector<VectorNf, 3> Vector3xNf;
typedef Vector<VectorNi, 3> Vector3xNi;
typedef Vector<VectorNf, 4> Vector4xNf;
typedef Vector<VectorNi, 4> Vector4xNi;


//////////////////////////////////////////////////////////
// Some additional conversion functions that are useful //
//////////////////////////////////////////////////////////

    /*
template <int M> Vector<Vector<float, M>, 4> expand(const Vector4f& v)
{ return Vector<Vector<float, M>, 4>(Vector<float, M>(v.x()), Vector<float, M>(v.y()), Vector<float, M>(v.z()), Vector<float, M>(v.w())); }
template <int M> Vector<Vector<float, M>, 3> expand(const Vector3f& v)
{ return Vector<Vector<float, M>, 3>(Vector<float, M>(v.x()), Vector<float, M>(v.y()), Vector<float, M>(v.z())); }
template <int M> Vector<Vector<float, M>, 2> expand(const Vector2f& v)
{ return Vector<Vector<float, M>, 2>(Vector<float, M>(v.x()), Vector<float, M>(v.y())); }

template <int M> Vector<Vector<int32, M>, 4> expand(const Vector4i& v)
{ return Vector<Vector<int32, M>, 4>(Vector<int32, M>(v.x()), Vector<int32, M>(v.y()), Vector<int32, M>(v.z()), Vector<int32, M>(v.w())); }
template <int M> Vector<Vector<int32, M>, 3> expand(const Vector3i& v)
{ return Vector<Vector<int32, M>, 3>(Vector<int32, M>(v.x()), Vector<int32, M>(v.y()), Vector<int32, M>(v.z())); }
template <int M> Vector<Vector<int32, M>, 2> expand(const Vector2i& v)
{ return Vector<Vector<int32, M>, 2>(Vector<int32, M>(v.x()), Vector<int32, M>(v.y())); }

static FINLINE Vector4xNf expandByN(const Vector4f& v)
{ return Vector4xNf(VectorNf(v.x()), VectorNf(v.y()), VectorNf(v.z()), VectorNf(v.w())); }
static FINLINE Vector3xNf expandByN(const Vector3f& v)
{ return Vector3xNf(VectorNf(v.x()), VectorNf(v.y()), VectorNf(v.z())); }
static FINLINE Vector2xNf expandByN(const Vector2f& v)
{ return Vector2xNf(VectorNf(v.x()), VectorNf(v.y())); }

static FINLINE Vector4xNi expandByN(const Vector4i& v)
{ return Vector4xNi(VectorNi(v.x()), VectorNi(v.y()), VectorNi(v.z()), VectorNi(v.w())); }
static FINLINE Vector3xNi expandByN(const Vector3i& v)
{ return Vector3xNi(VectorNi(v.x()), VectorNi(v.y()), VectorNi(v.z())); }
static FINLINE Vector2xNi expandByN(const Vector2i& v)
{ return Vector2xNi(VectorNi(v.x()), VectorNi(v.y())); }
*/


template <int N, class T> FINLINE Vector<T, N>
lerp(const Vector<T, N>& coord0, const Vector<T, N>& coord1, const Vector<float, N>& t)
{ return (ONE<Vector<float, N> >() - t) * coord0 + t * coord1; }

FINLINE VectorNf lerp(float coord0, float coord1, const VectorNf& t)
{ return (VectorNf(1.0f) - t) * coord0 + t*coord1; }



//////////////////////////////////////////////////////////////////////////////
//                    Vector Memory Operation Interfaces                    //
//////////////////////////////////////////////////////////////////////////////

/// Unmasked gather and scatter
template <class RT, class MT, uint8 WIDTH> FINLINE
const Vector<RT, WIDTH> GATHER( const MT * root, const Vector<int32, WIDTH>& indices )
{ return Vector<RT, WIDTH>::gather(root, indices); }

template <class RT, class MT, uint8 WIDTH> FINLINE
void SCATTER( MT * root, const Vector<int32, WIDTH>& indices, const Vector<RT, WIDTH>& input )
{ input.scatter(root, indices); }

/// Masked gather and scatter
template <class RT, class MT, uint8 WIDTH> FINLINE const Vector<RT, WIDTH> GATHER(
   const MT * root,
   const Vector<int32, WIDTH>& indices,
   const typename Vector<RT, WIDTH>::Mask& mask)
{ return Vector<RT, WIDTH>::gather(root, indices, mask); }

template <class RT, class MT, uint8 WIDTH> FINLINE void SCATTER(
   MT * root,
   const Vector<int32, WIDTH>& indices,
   const Vector<RT, WIDTH>& input,
   const typename Vector<RT, WIDTH>::Mask& mask)
{ input.scatter(root, indices, mask); }

}; // namespace Burns



#include "AffineFrame.h"
#include "Halton.h"
#include "Matrix.h"
#include "OrthogonalFrame.h"
#include "Quaternion.h"
#include "Rotation.h"

#ifdef ___WINDOWS_NT
#pragma warning(pop)
#endif


#endif // MATHEMATICS_H



