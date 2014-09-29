#ifndef MATHEMATICS_H
#define MATHEMATICS_H

#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <cstdlib>
#include <limits>
#include <string>
#include <sstream>

////////////////////////////
// Mathematical Constants //
////////////////////////////

/// _INF represents negative infinity

namespace Mathematics
{
   namespace FLOAT32
   {
      /// Constants
      static const float _INF    = -std::numeric_limits<float>::infinity();
      static const float INF     =  std::numeric_limits<float>::infinity();
      static const float MAXV    =  std::numeric_limits<float>::max();
      static const float EPSILON =  std::numeric_limits<float>::epsilon();
      static const float E       =  2.7182818284590452353602875f;
      static const float PI      =  3.1415926535897932384626433832795f;
      static const float RCP_PI  =  0.31830988618379067154f;
      static const float PHI     =  1.6180339887498948482045868343656f;
      static const float SQRT_2  =  1.4142135623730950488016887242097f;
   };

   namespace FLOAT64
   {
      /// Constants
      static const double _INF    = -std::numeric_limits<double>::infinity();
      static const double INF     =  std::numeric_limits<double>::infinity();
      static const double MAXV    =  std::numeric_limits<double>::max();
      static const double EPSILON =  std::numeric_limits<double>::epsilon();
      static const double E       =  2.7182818284590452353602875;
      static const double PI      =  3.1415926535897932384626433832795;
      static const double RCP_PI  =  0.31830988618379067154;
      static const double PHI     =  1.6180339887498948482045868343656;
      static const double SQRT_2  =  1.4142135623730950488016887242097;
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
   template <int N> inline int NextPowerOf2Multiple(int i)
   { return ((i + N-1) >> ILOG2<N>::value) << ILOG2<N>::value; }

   /// Integer base-2 logarithm functions
   inline uint32_t ILog2(uint32_t x)
   {
      int r = 0;
      while ((x >> r) != 0) r++;
      return r-1; // returns -1 for x == 0, floor(log2(x)) otherwise
   }

   inline uint64_t ILog2(uint64_t x)
   {
      int r = 0;
      while ((x >> r) != 0) r++;
      return r-1; // returns -1 for x == 0, floor(log2(x)) otherwise
   }

   /// Returns true if the argument is a power of two. Use only on integral
   /// types (int8, uint16, etc)
   template <typename T> inline bool IsPowerOfTwo(T n)
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
   template <class T> inline T NextLargestPowerOfTwo(T i)
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
   template <class T> inline T Lerp(T coord0, T coord1, float t)
   { return (1.0f - t) * coord0 + t*coord1; }

   //////////////////////////////////////////////////////////////////////////////
   //                                 TypeInfo                                 //
   //////////////////////////////////////////////////////////////////////////////

   /// \struct TypeInfo, provides basic traits and information about both
   /// primitive and user-defined types. This definition is the canonical
   /// form, and provides all the values and interfaces which should be
   /// provided by all types which participate in the system
   template <class T> class TypeInfo
   {
   public:
      static const bool Integral = false;       // Is the type fixed-point
      static const bool Unsigned = false;       // Is the type unsigned?
      static const int  Rank = 0;               // Scalars are rank 0 tensors
      static const int  Width = 1;              // Scalars have width 1

      typedef bool Mask;
   };

   template <> class TypeInfo<float>
   {
   public:
      static const bool Integral = false;
      static const bool Unsigned = false;
      static const int  Rank = 0;
      static const int  Width = 1;
      typedef float ElementType;
      typedef bool Mask;
   };
   template <> class TypeInfo<double>
   {
   public:
      static const bool Integral = false;
      static const bool Unsigned = false;
      static const int  Rank = 0;
      static const int  Width = 1;
      typedef double ElementType;
      typedef bool Mask;
   };
   template <> class TypeInfo<uint8_t>
   {
   public:
      static const bool Integral = true;
      static const bool Unsigned = true;
      static const int  Rank = 0;
      static const int  Width = 1;
      typedef uint8_t ElementType;
      typedef bool Mask;
   };
   template <> class TypeInfo<uint16_t>
   {
   public:
      static const bool Integral = true;
      static const bool Unsigned = true;
      static const int  Rank = 0;
      static const int  Width = 1;
      typedef uint16_t ElementType;
      typedef bool Mask;
   };
   template <> class TypeInfo<uint32_t>
   {
   public:
      static const bool Integral = true;
      static const bool Unsigned = true;
      static const int  Rank = 0;
      static const int  Width = 1;
      typedef uint32_t ElementType;
      typedef bool Mask;
   };
   template <> class TypeInfo<uint64_t>
   {
   public:
      static const bool Integral = true;
      static const bool Unsigned = true;
      static const int  Rank = 0;
      static const int  Width = 1;
      typedef uint64_t ElementType;
      typedef bool Mask;
   };
   template <> class TypeInfo<int8_t>
   {
   public:
      static const bool Integral = true;
      static const bool Unsigned = false;
      static const int  Rank = 0;
      static const int  Width = 1;
      typedef int8_t ElementType;
      typedef bool Mask;
   };
   template <> class TypeInfo<int16_t>
   {
   public:
      static const bool Integral = true;
      static const bool Unsigned = false;
      static const int  Rank = 0;
      static const int  Width = 1;
      typedef int16_t ElementType;
      typedef bool Mask;
   };
   template <> class TypeInfo<int32_t>
   {
   public:
      static const bool Integral = true;
      static const bool Unsigned = false;
      static const int  Rank = 0;
      static const int  Width = 1;
      typedef int32_t ElementType;
      typedef bool Mask;
   };

   template <> class TypeInfo<int64_t>
   {
   public:
      static const bool Integral = true;
      static const bool Unsigned = false;
      static const int  Rank = 0;
      static const int  Width = 1;
      typedef int64_t ElementType;
      typedef bool Mask;
   };


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
      static inline T zero()           { return (T) 0.0; }
      static inline T one()            { return (T) 1.0; }
      static inline T minusOne()       { return (T) -1.0; }
      static inline T inf()            { return (T) FLOAT64::INF; }
      static inline T _inf()           { return (T) FLOAT64::_INF; }
      static inline T maxv()           { return (T) FLOAT64::MAXV; }
      static inline T e()              { return (T) FLOAT64::E; }
      static inline T pi()             { return (T) FLOAT64::PI; }
      static inline T rcpPi()          { return (T) FLOAT64::RCP_PI; }
      static inline T phi()            { return (T) FLOAT64::PHI; }
      static inline T sqrt2()          { return (T) FLOAT64::SQRT_2; }

      static inline T cbrt(T x)        { return (T) ::cbrt((double)x); }
      static inline T sqrt(T x)        { return (T) ::sqrt((double)x); }
      static inline T rcpSqrt(T x)     { return (T) (1.0 / ::sqrt((double)x)); }
      static inline T rcp(T x)         { return (T) (1.0 / (double)x); }
      static inline T cos(T x)         { return (T) ::cos((double)x); }
      static inline T sin(T x)         { return (T) ::sin((double)x); }
      static inline T tan(T x)         { return (T) ::tan((double)x); }
      static inline T pow(T x, T y)    { return (T) ::pow((double)x, (double)y); }
      static inline T exp(T x)         { return (T) ::exp((double)x); }
      static inline T log(T x)         { return (T) ::log((double)x); }
      static inline T log2(T x)        { return (T) ::log2((double)x); }
      static inline T log10(T x)       { return (T) ::log10((double)x); }
      static inline T abs(T x)         { return (T) ::fabs((double)x); }
      static inline T acos(T x)        { return (T) ::acos((double)x); }
      static inline T asin(T x)        { return (T) ::asin((double)x); }
      static inline T atan2(T x, T y)  { return (T) ::atan2((double)x, (double)y); }
      static inline T floor(T x)       { return (T) ::floor((double)x); }
      static inline T ceil(T x)        { return (T) ::ceil((double)x); }
      static inline T sign(T x)        { return (T)(x > 0.0 ? 1.0 : (x < 0.0 ? -1.0 : 0.0)); }

      // Clamp from below by zero, above by the given value
      static inline T clampz(T x, T u) { return (x < zero() ? 0.0f : (x > u ? u : x)); }

      static inline T min(T x, T y)          { return (x < y ? x : y); }
      static inline T max(T x, T y)          { return (x > y ? x : y); }
      static inline T min3(T x, T y, T z)    { return min(x, min(y, z)); }
      static inline T max3(T x, T y, T z)    { return max(x, max(y, z)); }
   };


   /// Specialization for uint8_ts
   template <> struct MATHEMATICS<uint8_t>
   {
      static inline uint8_t zero()          { return 0u; }
      static inline uint8_t one()           { return 1u; }
      static inline uint8_t inf()           { return 0xffu; }
      static inline uint8_t _inf()          { return 0x00u; }

      static inline uint8_t clampz(uint8_t x, uint8_t u)
      { return (x > u ? u : x); }

      static inline uint8_t log2(uint8_t x)                   { return (uint8_t)ILog2((uint32_t)x); }

      //static inline uint8_t sign(uint8_t x)                   { return 0u; }
      static inline uint8_t min(uint8_t x, uint8_t y)           { return (x < y ? x : y); }
      static inline uint8_t max(uint8_t x, uint8_t y)           { return (x > y ? x : y); }
      static inline uint8_t min3(uint8_t x, uint8_t y, uint8_t z) { return min(x, min(y, z)); }
      static inline uint8_t max3(uint8_t x, uint8_t y, uint8_t z) { return max(x, max(y, z)); }
   };

   /// Specialization for uint16_ts
   template <> struct MATHEMATICS<uint16_t>
   {
      static inline uint16_t zero()          { return 0u; }
      static inline uint16_t one()           { return 1u; }
      static inline uint16_t inf()           { return 0xffffu; }
      static inline uint16_t _inf()          { return 0x0000u; }

      static inline uint16_t clampz(uint16_t x, uint16_t u)
      { return (x > u ? u : x); }

      static inline uint16_t log2(uint16_t x)                      { return (uint16_t)ILog2((uint32_t)x); }

      //static inline uint16_t sign(uint16_t x)                     { return 0u; }
      static inline uint16_t min(uint16_t x, uint16_t y)            { return (x < y ? x : y); }
      static inline uint16_t max(uint16_t x, uint16_t y)            { return (x > y ? x : y); }
      static inline uint16_t min3(uint16_t x, uint16_t y, uint16_t z) { return min(x, min(y, z)); }
      static inline uint16_t max3(uint16_t x, uint16_t y, uint16_t z) { return max(x, max(y, z)); }

      static inline uint16_t maxv() { return 0xffffu; }
   };

   /// Specialization for uint32_ts
   template <> struct MATHEMATICS<uint32_t>
   {
      static inline uint32_t zero()          { return 0u; }
      static inline uint32_t one()           { return 1u; }
      static inline uint32_t inf()           { return 0xffffffffu; }
      static inline uint32_t _inf()          { return 0x00000000u; }

      static inline uint32_t clampz(uint32_t x, uint32_t u)
      { return (x > u ? u : x); }

      static inline uint32_t log2(uint32_t x)                      { return (uint32_t)ILog2(x); }

      //static inline uint16_t sign(uint32_t x)                     { return 0u; }
      static inline uint32_t min(uint32_t x, uint32_t y)            { return (x < y ? x : y); }
      static inline uint32_t max(uint32_t x, uint32_t y)            { return (x > y ? x : y); }
      static inline uint32_t min3(uint32_t x, uint32_t y, uint32_t z) { return min(x, min(y, z)); }
      static inline uint32_t max3(uint32_t x, uint32_t y, uint32_t z) { return max(x, max(y, z)); }

      static inline uint32_t maxv() { return 0xffffffffu; }
   };

   /// Specialization for uint64_ts
   template <> struct MATHEMATICS<uint64_t>
   {
      static inline uint64_t zero()          { return 0ULL; }
      static inline uint64_t one()           { return 1ULL; }
      static inline uint64_t inf()           { return 0xffffffffffffffffULL; }
      static inline uint64_t _inf()          { return 0x0000000000000000ULL; }

      static inline uint64_t clampz(uint64_t x, uint64_t u)
      { return (x > u ? u : x); }

      static inline uint64_t log2(uint64_t x)                      { return ILog2(x); }

      //static inline uint16_t sign(uint64_t x)                     { return 0ULL; }
      static inline uint64_t min(uint64_t x, uint64_t y)            { return (x < y ? x : y); }
      static inline uint64_t max(uint64_t x, uint64_t y)            { return (x > y ? x : y); }
      static inline uint64_t min3(uint64_t x, uint64_t y, uint64_t z) { return min(x, min(y, z)); }
      static inline uint64_t max3(uint64_t x, uint64_t y, uint64_t z) { return max(x, max(y, z)); }

      static inline uint64_t maxv() { return 0xffffffffffffffffULL; }
   };

   /// Specialization for int8s
   template <> struct MATHEMATICS<int8_t>
   {
      static inline int8_t zero()          { return 0; }
      static inline int8_t one()           { return 1; }
      static inline int8_t minusOne()      { return -1; }
      static inline int8_t inf()           { return int8_t(0x7f); }
      static inline int8_t _inf()          { return int8_t(0x81); }

      static inline int8_t clampz(int8_t x, int8_t u)
      { return (x < 0 ? 0 : (x > u ? u : x)); }
      static inline int8_t sign(int8_t x) { return (x < 0 ? -1 : (x > 0 ? 1 : 0)); }

      //static inline int8_t sign(int8_t x) { return (uint8_t(x) >> 7); } // right logical shift
      static inline int8_t abs(int8_t x)                     { return (int8_t)::abs(x); }
      static inline int8_t min(int8_t x, int8_t y)             { return (x < y ? x : y); }
      static inline int8_t max(int8_t x, int8_t y)             { return (x > y ? x : y); }
      static inline int8_t min3(int8_t x, int8_t y, int8_t z)    { return min(x, min(y, z)); }
      static inline int8_t max3(int8_t x, int8_t y, int8_t z)    { return max(x, max(y, z)); }

      static inline int8_t maxv() { return 0x7f; }
   };

   /// Specialization for int8s
   template <> struct MATHEMATICS<int16_t>
   {
      static inline int16_t zero()          { return 0; }
      static inline int16_t one()           { return 1; }
      static inline int16_t minusOne()      { return -1; }
      static inline int16_t inf()           { return (int16_t)0x7fff; }
      static inline int16_t _inf()          { return (int16_t)0x8001; }

      static inline int16_t clampz(int16_t x, int16_t u)
      { return (x < 0 ? 0 : (x > u ? u : x)); }
      static inline int16_t sign(int16_t x) { return (x < 0 ? -1 : (x > 0 ? 1 : 0)); }

      //static inline int8_t sign(int16_t x) { return (uint16_t_t(x) >> 15); } // right logical shift
      static inline int16_t abs(int16_t x)                    { return (int16_t)::abs(x); }
      static inline int16_t min(int16_t x, int16_t y)           { return (x < y ? x : y); }
      static inline int16_t max(int16_t x, int16_t y)           { return (x > y ? x : y); }
      static inline int16_t min3(int16_t x, int16_t y, int16_t z) { return min(x, min(y, z)); }
      static inline int16_t max3(int16_t x, int16_t y, int16_t z) { return max(x, max(y, z)); }

      static inline int16_t maxv() { return 0x7fff; }
   };

   /// Specialization for int8s
   template <> struct MATHEMATICS<int32_t>
   {
      static inline int32_t zero()          { return 0; }
      static inline int32_t one()           { return 1; }
      static inline int32_t minusOne()      { return -1; }
      static inline int32_t inf()           { return 0x7fffffff; }
      static inline int32_t _inf()          { return 0x80000001; }

      static inline int32_t clampz(int32_t x, int32_t u)
      { return (x < zero() ? zero() : (x > u ? u : x)); }
      static inline int32_t sign(int32_t x) { return (x < 0 ? -1 : (x > 0 ? 1 : 0)); }

      //static inline int8_t sign(int32_t x) { return (uint32_t_t(x) >> 31); } // right logical shift
      static inline int32_t abs(int32_t x) { return ::abs(x); }
      static inline int32_t min(int32_t x, int32_t y)           { return (x < y ? x : y); }
      static inline int32_t max(int32_t x, int32_t y)           { return (x > y ? x : y); }
      static inline int32_t min3(int32_t x, int32_t y, int32_t z) { return min(x, min(y, z)); }
      static inline int32_t max3(int32_t x, int32_t y, int32_t z) { return max(x, max(y, z)); }

      static inline int32_t maxv() { return 0x7fffffff; }
   };

   /// Specialization for int64_ts
   template <> struct MATHEMATICS<int64_t>
   {
      static inline int64_t zero()          { return 0LL; }
      static inline int64_t one()           { return 1LL; }
      static inline int64_t minusOne()      { return -1LL; }
      static inline int64_t inf()           { return 0x7fffffffffffffffLL; }
      static inline int64_t _inf()          { return 0x8000000000000001LL; }

      static inline int64_t clampz(int64_t x, int64_t u)
      { return (x < zero() ? zero() : (x > u ? u : x)); }
      static inline int64_t sign(int64_t x) { return (x < 0 ? -1 : (x > 0 ? 1 : 0)); }

      //static inline int8_t sign(int64_t x) { return (uint64_t(x) >> 63); } // right logical shift
      static inline int64_t abs(int64_t x) { return ::abs(x); }
      static inline int64_t min(int64_t x, int64_t y)           { return (x < y ? x : y); }
      static inline int64_t max(int64_t x, int64_t y)           { return (x > y ? x : y); }
      static inline int64_t min3(int64_t x, int64_t y, int64_t z) { return min(x, min(y, z)); }
      static inline int64_t max3(int64_t x, int64_t y, int64_t z) { return max(x, max(y, z)); }

      static inline int64_t maxv() { return 0x7fffffffffffffffLL; }
   };



   // Global namespace wrappers for the Math class namespace...
   template <class T> inline T ZERO()           { return MATHEMATICS<T>::zero(); }
   template <class T> inline T ONE()            { return MATHEMATICS<T>::one(); }
   template <class T> inline T MINUS_ONE()      { return MATHEMATICS<T>::minusOne(); }
   template <class T> inline T INF()            { return MATHEMATICS<T>::inf(); }
   template <class T> inline T _INF()           { return MATHEMATICS<T>::_inf(); }
   template <class T> inline T MAXV()           { return MATHEMATICS<T>::maxv(); }
   template <class T> inline T E()              { return MATHEMATICS<T>::e(); }
   template <class T> inline T PI()             { return MATHEMATICS<T>::pi(); }
   template <class T> inline T RCP_PI()         { return MATHEMATICS<T>::rcpPi(); }
   template <class T> inline T PHI()            { return MATHEMATICS<T>::phi(); }
   template <class T> inline T SQRT2()          { return MATHEMATICS<T>::sqrt2(); }

   template <class T> inline T CBRT(T x)        { return MATHEMATICS<T>::cbrt(x); }
   template <class T> inline T SQRT(T x)        { return MATHEMATICS<T>::sqrt(x); }
   template <class T> inline T RCP_SQRT(T x)    { return MATHEMATICS<T>::rcpSqrt(x); }
   template <class T> inline T RCP(T x)         { return MATHEMATICS<T>::rcp(x); }
   template <class T> inline T COS (T x)        { return MATHEMATICS<T>::cos(x); }
   template <class T> inline T SIN (T x)        { return MATHEMATICS<T>::sin(x); }
   template <class T> inline T TAN (T x)        { return MATHEMATICS<T>::tan(x); }
   template <class T> inline T POW (T x, T y)   { return MATHEMATICS<T>::pow(x, y); }
   template <class T> inline T EXP (T x)        { return MATHEMATICS<T>::exp(x); }
   template <class T> inline T LOG (T x)        { return MATHEMATICS<T>::log(x); }
   template <class T> inline T LOG2 (T x)       { return MATHEMATICS<T>::log2(x); }

   #ifdef ABS
   #undef ABS
   #endif
   template <class T> inline T ABS (T x)         { return MATHEMATICS<T>::abs(x); }
   template <class T> inline T LOG10 (T x)       { return MATHEMATICS<T>::log10(x); }
   template <class T> inline T ACOS (T x)        { return MATHEMATICS<T>::acos(x); }
   template <class T> inline T ASIN (T x)        { return MATHEMATICS<T>::asin(x); }
   template <class T> inline T ATAN2 (T x, T y)  { return MATHEMATICS<T>::atan2(x,y); }
   template <class T> inline T FLOOR (T x)       { return MATHEMATICS<T>::floor(x); }
   template <class T> inline T CEILING (T x)     { return MATHEMATICS<T>::ceil(x); }
   template <class T> inline T CLAMPZ (T x, T u) { return MATHEMATICS<T>::clampz(x, u); }
   template <class T> inline T SIGN (T x)        { return MATHEMATICS<T>::sign(x); }


   #ifdef MIN
   #undef MIN
   #endif
   #ifdef MAX
   #undef MAX
   #endif
   template <class T> inline T MIN(T x, T y)       { return MATHEMATICS<T>::min(x, y); }
   template <class T> inline T MAX(T x, T y)       { return MATHEMATICS<T>::max(x, y); }
   template <class T> inline T MIN3(T x, T y, T z) { return MATHEMATICS<T>::min3(x, y, z); }
   template <class T> inline T MAX3(T x, T y, T z) { return MATHEMATICS<T>::max3(x, y, z); }



   /// Conversion between degrees and radians. Defined for float and double
   template <class T> inline T DEG_TO_RAD(T x)
   { return (x) * (PI<T>()   / T(180.0)); }
   template <class T> inline T RAD_TO_DEG(T x)
   { return (x) * 180.0 * RCP_PI<double>(); }
   template <> inline float DEG_TO_RAD(float x)
   { return (x) * (PI<float>()   / 180.0f); }
   template <> inline float RAD_TO_DEG(float x)
   { return (x) * 180.0f * RCP_PI<float>(); }



   /// A class which generates a random prime 32-bit number upon request. The
   /// interface consists of one static method which accepts an integer input
   /// which is incremented until a prime is found.
   class Prime
   {
    private:

      static inline uint32_t isqrt(uint32_t i)
      {
         uint32_t r = 0, rnew = 1, rold = r;
         do
         {
            rold = r;           r = rnew;
            rnew = (r + (i/r)); rnew >>= 1;
         } while (rold != rnew);

         return rnew;
      }

   public:

      /// Returns the smallest prime number greater-than or equal to \param p
      static inline uint32_t NextPrime(uint32_t p)
      {
         while (!IsPrime(p)) ++p;
         return p;
      }

      /// Returns true if the integer i is prime, false otherwise
      static inline bool IsPrime(uint32_t i)
      {
         uint32_t si = isqrt(i);
         for (uint32_t j = 2; j <= si; j++) if (i % j == 0) return false;
         return true;
      }
   };

}; // namespace Mathematics



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


namespace Mathematics
{
   /// Specialization for single-precision floats. This used to come earlier in
   /// the file here, but was moved after we included SSE so that we
   /// could use SSE intrinsics for some of these special functions
   template <> struct MATHEMATICS<float>
   {
      static inline float zero()          { return 0.0f; }
      static inline float one()           { return 1.0f; }
      static inline float minusOne()      { return -1.0f; }
      static inline float inf()           { return FLOAT32::INF; }
      static inline float _inf()          { return FLOAT32::_INF; }
      static inline float maxv()          { return FLOAT32::MAXV; }
      static inline float e()             { return FLOAT32::E; }
      static inline float pi()            { return FLOAT32::PI; }
      static inline float rcpPi()         { return FLOAT32::RCP_PI; }
      static inline float phi()           { return FLOAT32::PHI; }
      static inline float sqrt2()         { return FLOAT32::SQRT_2; }

      static inline float cbrt(float x)            { return ::cbrtf(x); }
      static inline float sqrt(float x)            { return ::sqrtf(x); }
      static inline float rcpSqrt(float x)         { return (1.0f / ::sqrtf(x)); }
      static inline float rcp(float x)             { return (1.0f / x); }
      static inline float cos(float x)             { return ::cosf(x); }
      static inline float sin(float x)             { return ::sinf(x); }
      static inline float tan(float x)             { return ::tanf(x); }
      static inline float pow(float x, float y)    { return ::powf(x, y); }
      static inline float exp(float x)             { return ::expf(x); }
      static inline float log(float x)             { return ::logf(x); }
      static inline float log2(float x)            { return ::log2f(x); }
      static inline float log10(float x)           { return ::log10f(x); }
      static inline float abs(float x)             { return ::fabsf(x); }
      static inline float acos(float x)            { return ::acosf(x); }
      static inline float asin(float x)            { return ::asinf(x); }
      static inline float atan2(float x, float y)  { return ::atan2f(x, y); }

   #if defined(___SSE4)
      static inline float floor(float x)           { return SSERegister(x).fpFloor().fpExtract<0>(); }
      static inline float ceil(float x)            { return SSERegister(x).fpCeiling().fpExtract<0>(); }
      static inline float clampz(float x, float u) { return SSERegister(x).fpClamp(SSERegister::zeroes(), SSERegister(u)).fpExtract<0>(); }
   #else
      static inline float floor(float x)           { return ::floorf(x); }
      static inline float ceil(float x)            { return ::ceilf(x); }
      static inline float clampz(float x, float u) { return (x < 0.0f ? 0.0f : (x > u ? u : x)); }
   #endif

      static inline float sign(float x)            { return (x > 0.0f ? 1.0f : (x < 0.0f ? -1.0f : 0.0f)); }
      static inline float min(float x, float y)           { return (x < y ? x : y); }
      static inline float max(float x, float y)           { return (x > y ? x : y); }
      static inline float min3(float x, float y, float z) { return min(x, min(y, z)); }
      static inline float max3(float x, float y, float z) { return max(x, max(y, z)); }
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

   typedef Vector<int32_t, 2>  Vector2i;
   typedef Vector<int32_t, 3>  Vector3i;
   typedef Vector<int32_t, 4>  Vector4i;
   typedef Vector<int32_t, 8>  Vector8i;
   typedef Vector<int32_t, 16> Vector16i;
   typedef Vector<int32_t, 64> Vector64i;

   typedef Vector<int64_t, 2>  Vector2l;
   typedef Vector<int64_t, 3>  Vector3l;
   typedef Vector<int64_t, 4>  Vector4l;
   typedef Vector<int64_t, 8>  Vector8l;
   typedef Vector<int64_t, 16> Vector16l;
   typedef Vector<int64_t, 64> Vector64l;


   typedef Vector<Vector<float, 4>, 2>   Vector2x4f;
   typedef Vector<Vector<float, 8>, 2>   Vector2x8f;
   typedef Vector<Vector<float, 16>, 2>  Vector2x16f;
   typedef Vector<Vector<float, 64>, 2>  Vector2x64f;

   typedef Vector<Vector<int32_t, 4>, 2>   Vector2x4i;
   typedef Vector<Vector<int32_t, 8>, 2>   Vector2x8i;
   typedef Vector<Vector<int32_t, 16>, 2>  Vector2x16i;
   typedef Vector<Vector<int32_t, 64>, 2>  Vector2x64i;


   typedef Vector<Vector<float, 2>, 3>   Vector3x2f;
   typedef Vector<Vector<float, 3>, 3>   Vector3x3f;
   typedef Vector<Vector<float, 4>, 3>   Vector3x4f;
   typedef Vector<Vector<float, 8>, 3>   Vector3x8f;
   typedef Vector<Vector<float, 16>, 3>  Vector3x16f;
   typedef Vector<Vector<float, 64>, 3>  Vector3x64f;

   typedef Vector<Vector<int32_t, 2>, 3>   Vector3x2i;
   typedef Vector<Vector<int32_t, 3>, 3>   Vector3x3i;
   typedef Vector<Vector<int32_t, 4>, 3>   Vector3x4i;
   typedef Vector<Vector<int32_t, 8>, 3>   Vector3x8i;
   typedef Vector<Vector<int32_t, 16>, 3>  Vector3x16i;
   typedef Vector<Vector<int32_t, 64>, 3>  Vector3x64i;

   typedef Vector<Vector<float, 2>, 4>   Vector4x2f;
   typedef Vector<Vector<float, 3>, 4>   Vector4x3f;
   typedef Vector<Vector<float, 4>, 4>   Vector4x4f;
   typedef Vector<Vector<float, 8>, 4>   Vector4x8f;
   typedef Vector<Vector<float, 16>, 4>  Vector4x16f;
   typedef Vector<Vector<float, 64>, 4>  Vector4x64f;

   typedef Vector<Vector<int32_t, 2>, 4>   Vector4x2i;
   typedef Vector<Vector<int32_t, 3>, 4>   Vector4x3i;
   typedef Vector<Vector<int32_t, 4>, 4>   Vector4x4i;
   typedef Vector<Vector<int32_t, 8>, 4>   Vector4x8i;
   typedef Vector<Vector<int32_t, 16>, 4>  Vector4x16i;
   typedef Vector<Vector<int32_t, 64>, 4>  Vector4x64i;

   /// Here we define a "canonical" computational vector to be used whenever we
   /// simply want to use the native machine vector for data-parallel performance
   #if defined(___AVX)
   typedef Vector8f VectorNf;
   typedef Vector8i VectorNi;
   typedef Vector8l VectorNl;
   typedef Mask8    MaskN;
   #define SIMD_WIDTH 8
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

   template <int N, class T> inline Vector<T, N>
   lerp(const Vector<T, N>& coord0, const Vector<T, N>& coord1, const Vector<float, N>& t)
   { return (ONE<Vector<float, N> >() - t) * coord0 + t * coord1; }

   inline VectorNf lerp(float coord0, float coord1, const VectorNf& t)
   { return (VectorNf(1.0f) - t) * coord0 + t*coord1; }

   template <class T> inline T lerp(const T& t0, const T& t1, float f)
   { return (1.0f-f) * t0 + f * t1; }


   //////////////////////////////////////////////////////////////////////////////
   //                    Vector Memory Operation Interfaces                    //
   //////////////////////////////////////////////////////////////////////////////

   /// Unmasked gather and scatter
   template <class RT, class MT, uint8_t WIDTH> inline
   const Vector<RT, WIDTH> GATHER( const MT * root, const Vector<int32_t, WIDTH>& indices )
   { return Vector<RT, WIDTH>::gather(root, indices); }

   template <class RT, class MT, uint8_t WIDTH> inline
   void SCATTER( MT * root, const Vector<int32_t, WIDTH>& indices, const Vector<RT, WIDTH>& input )
   { input.scatter(root, indices); }

   /// Masked gather and scatter
   template <class RT, class MT, uint8_t WIDTH> inline const Vector<RT, WIDTH> GATHER(
      const MT * root,
      const Vector<int32_t, WIDTH>& indices,
      const typename Vector<RT, WIDTH>::Mask& mask)
   { return Vector<RT, WIDTH>::gather(root, indices, mask); }

   template <class RT, class MT, uint8_t WIDTH> inline void SCATTER(
      MT * root,
      const Vector<int32_t, WIDTH>& indices,
      const Vector<RT, WIDTH>& input,
      const typename Vector<RT, WIDTH>::Mask& mask)
   { input.scatter(root, indices, mask); }

}; // namespace Mathematics

#include "AffineFrame.h"
#include "Halton.h"
#include "Matrix.h"
#include "OrthogonalFrame.h"
#include "Quaternion.h"
#include "Rotation.h"

#endif // MATHEMATICS_H



