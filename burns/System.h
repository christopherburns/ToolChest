#pragma once

#ifndef SYSTEM_H
#define SYSTEM_H

#include <float.h>
#include <assert.h>
#include <math.h>


// OK This is where we set our defs based on microarchitecture, OS, and compiler
// and standardize what symbols throughout the library are used to signify these
// parameters

// All Core-defined preprocessor tokens identifying things like build configuration,
// platform, ISA, etc, begin with THREE UNDERSCORES: "___"

/// Resolve the OS family
#if defined(_WIN32)
   #define ___WINDOWS_NT
#elif defined(__APPLE__)
   #define ___APPLE
   #include <TargetConditionals.h>
   
   /// Note that these are always defined on apple systems, but set to 1 or 0
   /// to distinguish between states
   #if TARGET_OS_MAC && !TARGET_OS_IPHONE
      #define ___OS_X
   #elif TARGET_OS_IPHONE
      #define ___IOS
      #if TARGET_IPHONE_SIMULATOR
         #define ___IOS_SIMULATOR
      #endif
   #endif

#elif defined(LINUX)
   #define ___GNU_LINUX
#endif

/// Resolve the compiler family
#if defined(__INTEL_COMPILER)
   #define ___INTEL_COMPILER
#elif defined(_MSC_VER)
   #define ___MS_COMPILER
#elif defined(__GNUG__) || defined(__GNUC__)
   #define ___GNU_COMPILER
#elif defined(__clang__)
   #define ___CLANG
#endif

/// Resolve microarchitectural details, such as ISA extentions

/// SIMD ISA extentions and APIs here are mutually exclusive, depend on the
/// available hardware and software environment
#if defined(AVX)
   #define ___AVX
#endif

#if defined(SSE)
   #define ___SSE
   #define ___SSE4
#endif

/// 64-bit mode detection (long int, void* are 64 bits)
#if defined(___WINDOWS_NT)
   #if defined(_M_X64) || defined(__LP64__)
      #define ___X64
   #endif
#elif defined(___APPLE)
   #if TARGET_RT_64_BIT
      #define ___X64
   #endif
#endif





// We need to find a way to include the intrin.h crap without getting
// xmmintrin, emmintrin, etc


#if defined(___WINDOWS_NT)

   #ifndef WIN32_LEAN_AND_MEAN
   #define WIN32_LEAN_AND_MEAN
   #endif
   #include <intrin.h>
   #include <windows.h>

#elif defined(___APPLE)

   #include <mach/mach_time.h>
   #include <sys/sysctl.h>
   #include <sys/time.h>
   #include <libkern/OSAtomic.h>

#endif

// Debug mode serializes things
#if !defined(___DEBUG)
#define ___THREAD_PARALLEL
#endif


/////////////////////
// Primitive Types //
/////////////////////

namespace Burns
{

   /// Here we find out which OS symbol is defined, and then we define any other
   /// symbols which are appropriate for that type of system. Make sure the build
   /// system defines the appropriate tokens for your system.

   // Beware!! IA64 defines "long int" as 64-bit. int is 32-bit.
   // Revisit this code later, when needed, to make it more robust. Right now it
   // is rather ad hoc.
   #ifdef ___GNU_LINUX  // It is the build script's responsibility to define this

      typedef signed char              int8;
      typedef short int                int16;
	   #if defined(___X64)
      typedef int                      int32;
	   #else
      typedef long int                 int32;
	   #endif
      typedef long long int            int64;

      typedef unsigned char            uint8;
      typedef unsigned short int       uint16;

	   #if defined(___X64)
      typedef unsigned int             uint32;
	   #else
      typedef unsigned long int        uint32;
	   #endif
      typedef unsigned long long int   uint64;

      typedef float                    float32;
      typedef double                   float64;

	   #if defined(___X64)
      typedef long double              float128;
	   #endif

   #endif // LINUX

   #ifdef ___WINDOWS_NT

   typedef __int8					int8;
   typedef __int16				int16;
   typedef __int32				int32;
   typedef __int64				int64;

   typedef unsigned __int8		uint8;
   typedef unsigned __int16	uint16;
   typedef unsigned __int32	uint32;
   typedef unsigned __int64	uint64;

   typedef float					float32;
   typedef double					float64;

   #endif

   #ifdef ___APPLE /// Not sure if it matters whether OS X or iOS

   typedef signed char           int8;
   typedef short                 int16;
   typedef int                   int32;
   typedef long long             int64;

   typedef unsigned char         uint8;
   typedef unsigned short        uint16;
   typedef unsigned              uint32;
   typedef unsigned long long    uint64;

   typedef float                 float32;
   typedef double                float64;

   #endif // ___APPLE



   /// For absolute best performance, array indices should be in the machine's
   /// native integer type. For 64-bit mode we use 64-bit integers for indices,
   /// 32-bit for 32-bit mode
   #ifdef ___X64
   typedef int64 Index;
   #else
   typedef int32 Index;
   #endif
}

/////////////////////////////
// Disable Stupid Warnings //
/////////////////////////////

#if defined(___MS_COMPILER)
#pragma warning(disable : 4127)  // constant conditional expression
#pragma warning(disable : 4100)  // unreferenced formal parameter
#pragma warning(disable : 4189)  // local varialbe initialized but not referenced
#pragma warning(disable : 4324)  // structure was padded due to align( .. )
#pragma warning(disable : 4480)  // nonstandard extension used: enum type
#pragma warning(disable : 4512)  // assignment operator could not be generated
#pragma warning(disable : 4714)  // function marked "forceinline" not inlined
#endif



//////////////////////////////
// Macros and Substitutions //
//////////////////////////////

#ifdef INFINITY
#undef INFINITY
#endif
#define INFINITY   FLT_MAX

/// I hate these definitions
#if defined(___WINDOWS_NT)
   #ifdef TRUE
   #undef TRUE
   #endif
   #ifdef FALSE
   #undef FALSE
   #endif
#endif

#ifndef NULL
#define NULL      0
#endif


/// These are sometimes defined as macros (Windows...)
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

/// These are sometimes defined as macros (Windows...)
#ifdef MIN
#undef MIN
#endif
#ifdef MAX
#undef MAX
#endif


//! print function for assertions
#include <stdio.h>
//void printf(const char * format, ...);
inline void logAssert( const char* expression, const char* fileName, const int fileLine )
{ printf("Assertion failed : %s\n\n%s (%d)\n\n", expression, fileName, fileLine); }

#define always_Assert(expression, message) \
	do { \
		if ( !(expression) ) \
		{ \
			logAssert((message), __FILE__, __LINE__); \
			__debugbreak(); \
		} \
	} while ( false )
#define always_assert(expression) always_Assert((expression), #expression)


////////////////////////////
// Compiler Syntax Quirks //
////////////////////////////

/// MS-like compilers
#if defined(___WINDOWS_NT)   /// Windows

#define ___DLLEXPORT __declspec(dllexport)
#define NORETURN __declspec(noreturn)
#define BREAK __debugbreak();
#define INLINE inline             //__forceinline
#define FINLINE __forceinline
#define NOINLINE __declspec(noinline)
#define RESTRICT __restrict
#define FASTCALL __fastcall

#if defined(___MS_COMPILER)   // inline asm not supported by MS for x64 builds
#define ALIGNLOOP
#else
#define ALIGNLOOP __asm { align 16 }
#endif

#define THREAD_LOCAL __declspec(thread)

#if defined(___DEBUG) || defined(___SAFE)
#define ASSERT(x) always_assert(x)
#else
#define ASSERT(x)
#endif
#define ALWAYS_ASSERT(x) always_assert(x)

#pragma warning( disable:4996 ) // Warning about "unsecure" CRT routines like sprintf()
#pragma warning( disable:4800 ) // "Performance" warning about casting int to bool

/// GNU-like compilers, APPLE systems
#else

#define ___DLLEXPORT
#define BREAK asm ("int $3");
#define INLINE inline
#define FINLINE /*__attribute__((always_inline)) inline*/
#define NOINLINE __attribute__((noinline))
#define RESTRICT __restrict__
#define ALIGNLOOP
#define FASTCALL __attribute__((fastcall))
#define NORETURN
#define THREAD_LOCAL
#define ASSERT(x) assert(x)

#endif



///////////////////////////////
// Missing Windows Functions //
///////////////////////////////

#ifdef ___WINDOWS_NT

/// Math library routines (globally scoped)
FINLINE float cbrtf(const float& f) { return ::powf(f, 1.0f/3.0f); }
FINLINE double cbrt(const double& d) { return ::pow(d, 1.0/3.0); }
FINLINE float log2f(const float& f) { return ::log10f(f) / ::log10f(2.0f); }
FINLINE double log2(const double& d) { return ::log10(d) / ::log10(2.0); }

#endif


//////////////////////////////
// System Atomic Operations //
//////////////////////////////

namespace Burns
{
   
   ///////////////////////////
   // Mising Unix Functions //
   ///////////////////////////

   /// Missing on non-windows platforms
   #if !defined(___WINDOWS_NT)

      FINLINE int64 _abs64(int64 n)           { return ::abs(n); }
      FINLINE int64 _rotl64(int64 a, uint8 b) { return (a << b) | ((uint64)a >> (64-b)); }
      FINLINE int64 _rotr64(int64 a, uint8 b) { return ((uint64)a >> b) | (a << (64-b)); }
      FINLINE int32 _rotl(int32 a, uint8 b)   { return (a << b) | ((uint32)a >> (32-b)); }
      FINLINE int32 _rotr(int32 a, uint8 b)   { return ((uint32)a >> b) | (a << (32-b)); }
      FINLINE int16 _rotl16(int16 a, uint8 b) { return (a << b) | ((uint16)a >> (16-b)); }
      FINLINE int16 _rotr16(int16 a, uint8 b) { return ((uint16)a >> b) | (a << (16-b)); }
      FINLINE int8 _rotl8(int8 a, uint8 b)    { return (a << b) | ((uint8)a >> ( 8-b)); }
      FINLINE int8 _rotr8(int8 a, uint8 b)    { return ((uint8)a >> b) | (a << ( 8-b)); }

      FINLINE void __cpuid(uint32 CPUInfo[], const uint32 InfoType)
      { __asm__ __volatile__("cpuid" : "=a" (CPUInfo[0]), "=b" (CPUInfo[1]), "=c" (CPUInfo[2]), "=d" (CPUInfo[3]) : "a" (InfoType)); }
      FINLINE uint64 __rdtsc(void)
      { uint64 retval; __asm__ __volatile__("rdtsc" : "=A"(retval)); return retval; }

   #endif

   #if defined(___WINDOWS_NT)

      FINLINE int32 atomicDecrement(int32 volatile * v) { return _InterlockedDecrement((long volatile *)v); }
      FINLINE int32 atomicIncrement(int32 volatile * v) { return _InterlockedDecrement((long volatile *)v); }
      FINLINE int32 atomicExchange(int32 volatile * v, int32 a) { return _InterlockedExchange((long volatile *)v, (long)a); }

      FINLINE int32 atomicExchangeAdd(int32 volatile * v, int32 a)
      {
         /// Returns the INITIAL VALUE at v, before adding a - this is a postfix operator
         return _InterlockedExchangeAdd((long volatile *)v, (long)a);
      }

      FINLINE int32 atomicCompareExchange(int32 volatile * v, int32 a, int32 b) { return _InterlockedCompareExchange((long volatile *)v, a, b); }
      FINLINE int32 atomicOr(int32 volatile * v, int32  a) { return _InterlockedOr((long volatile *)v, (long)a); }
      FINLINE int32 atomicXor(int32 volatile * v, int32 a) { return _InterlockedXor((long volatile *)v, (long)a); }
      FINLINE int32 atomicAnd(int32 volatile * v, int32 a) { return _InterlockedAnd((long volatile *)v, (long)a); }

      #ifdef ___X64
         FINLINE int64 atomicDecrement(int64 volatile * v) { return _InterlockedDecrement64(v); }
         FINLINE int64 atomicIncrement(int64 volatile * v) { return _InterlockedIncrement64(v); }
         FINLINE int64 atomicExchange(int64 volatile * v, int64 a) { return _InterlockedExchange64(v, a); }
         FINLINE int64 atomicExchangeAdd(int64 volatile * v, int64 a) { return _InterlockedExchangeAdd64(v, a); }
         FINLINE int64 atomicCompareExchange(int64 volatile * v, int64 a, int64 b) { return _InterlockedCompareExchange64(v, a, b); }
         FINLINE int64 atomicOr (int64 volatile * v, int64 a) { return _InterlockedOr64 (v, a); }
         FINLINE int64 atomicXor(int64 volatile * v, int64 a) { return _InterlockedXor64(v, a); }
         FINLINE int64 atomicAnd(int64 volatile * v, int64 a) { return _InterlockedAnd64(v, a); }
      #endif

   #elif defined(___APPLE) || defined(___GNU_LINUX) /// unix-like systems...

   // All OSAtomic* operations return the value AFTER the operation has been performed,
   // so they are effectively prefix operators
   FINLINE int32 atomicDecrement(int32 volatile * v) { return OSAtomicDecrement32(v); }
   FINLINE int32 atomicIncrement(int32 volatile * v) { return OSAtomicIncrement32(v); }

   /// Figure out what to do with the rest later...
   FINLINE int32 atomicExchange(int32 volatile * v, int32 a) { ASSERT(false); return 0; }
   FINLINE int32 atomicExchangeAdd(int32 volatile * v, int32 a) { ASSERT(false); return 0; }
   FINLINE int32 atomicCompareExchange(int32 volatile * v, int32 a, int32 b) { ASSERT(false); return 0; }

   #ifdef ___X64
      FINLINE int64 atomicDecrement(int64 volatile * v) { ASSERT(false); return 0; }
      FINLINE int64 atomicIncrement(int64 volatile * v) { ASSERT(false); return 0; }
      FINLINE int64 atomicExchange(int64 volatile * v, int64 a) { ASSERT(false); return 0; }
      FINLINE int64 atomicExchangeAdd(int64 volatile * v, int64 a) { ASSERT(false); return 0; }
      FINLINE int64 atomicCompareExchange(int64 volatile * v, int64 a, int64 b) { ASSERT(false); return 0; }
   #endif

   #endif




/////////////////////////////
// Bit Counting Operations //
/////////////////////////////

static FINLINE int32 popcnt( const uint8 input ) { const uint8 table[0x100] = {
0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8 }; return int32(table[Index(input)]); }
static FINLINE int32 popcnt( const uint16 input ) { return popcnt(uint8 (input)) + popcnt(uint8 (input >>  8)); }
static FINLINE int32 popcnt( const uint32 input ) { return popcnt(uint16(input)) + popcnt(uint16(input >> 16)); }
static FINLINE int32 popcnt( const uint64 input ) { return popcnt(uint32(input)) + popcnt(uint32(input >> 32)); }

static FINLINE int8  popcnt( const int8  input ) { return (int8)popcnt(uint8 (input)); }
static FINLINE int32 popcnt( const int16 input ) { return (int32)popcnt(uint8 (input)) + popcnt(int8 (input >>  8)); }
static FINLINE int32 popcnt( const int32 input ) { return (int32)popcnt(uint16(input)) + popcnt(int16(input >> 16)); }
static FINLINE int32 popcnt( const int64 input ) { return (int32)popcnt(uint32(input)) + popcnt(int32(input >> 32)); }




///////////////
// Alignment //
///////////////

/// To check the alignment of a static structure S: Traits<S>::Alignment
/// To check the size of a static structure S: Traits<S>::Size

template <class T> class Traits
{
public:
   struct AlignmentFinder
   {
     char a;
     T b;
   };

   enum {Alignment = sizeof(AlignmentFinder) - sizeof(T)};
   enum {Size = sizeof(T)};
};


/// Align front, align back - place these like bookends around the declaration
/// that is to be aligned. The only legit parameter is the number of bytes to
/// be aligned with.
#if defined(___WINDOWS_NT)
#define ALIGNF(...)         __declspec(align(__VA_ARGS__))
#define ALIGNB(...)
#else /// GNU compilers, linux icc
#define ALIGNF(...)
#define ALIGNB(...)         __attribute__((aligned(__VA_ARGS__)))
#endif // _WIN32



// For affirming the unitlength-ness of normals
#define UNIT_LENGTH(x) (ABS(x.length() - 1.0f) < 2.0f * FLOAT32::EPSILON)


/// Use this in a class definition to prohibit the copy constructor and
/// standard assignment operators. This is primarily for so-called "Singleton"
/// classes.
/// \param c is the name of the class
#define FORBID_COPY_OR_ASSIGNMENT(c) \
   INLINE c(const c&) { STATIC_ASSERT(false); } \
   INLINE c& operator=(const c&) { STATIC_ASSERT(false); return *this; }

/// Nifty Template-Metaprogramming
///
/// For example, to make sure a templated function is never instantiated with a
/// Vector3, then call
///     TYPE_ASSERTION( !TypesAreEqual<T, Vector3>::value );
/// at the head of the function body.

#define TYPE_ASSERTION(a, b) { int dummy[ a , b ? 1 : -1]; }
template <class A, class B> struct TypesAreEqual { enum { value = false }; };
template <class A> struct TypesAreEqual<A, A> { enum { value = true }; };

/// Generalized static assertion. Any constant boolean expression will work.
template <bool> struct STATIC_ASSERTION_FAILURE;
template <> struct STATIC_ASSERTION_FAILURE<true> {};
#define STATIC_ASSERT(f) /*sizeof(STATIC_ASSERTION_FAILURE<(bool)(f)>);*/

template <typename T, typename U> FINLINE const T& cast(const U& from) { return *(T*)(&from); }
template <typename T, typename U> FINLINE       T& cast(      U& from) { return *(T*)(&from); }

//////////////////////////////////////////////////////////////////////////////
//                                 TypeInfo                                 //
//////////////////////////////////////////////////////////////////////////////

//template <int N, class T> class Mask;

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
template <> class TypeInfo<uint8>
{
public:
   static const bool Integral = true;
   static const bool Unsigned = true;
   static const int  Rank = 0;
   static const int  Width = 1;
   typedef uint8 ElementType;
   typedef bool Mask;
};
template <> class TypeInfo<uint16>
{
public:
   static const bool Integral = true;
   static const bool Unsigned = true;
   static const int  Rank = 0;
   static const int  Width = 1;
   typedef uint16 ElementType;
   typedef bool Mask;
};
template <> class TypeInfo<uint32>
{
public:
   static const bool Integral = true;
   static const bool Unsigned = true;
   static const int  Rank = 0;
   static const int  Width = 1;
   typedef uint32 ElementType;
   typedef bool Mask;
};
template <> class TypeInfo<uint64>
{
public:
   static const bool Integral = true;
   static const bool Unsigned = true;
   static const int  Rank = 0;
   static const int  Width = 1;
   typedef uint64 ElementType;
   typedef bool Mask;
};
template <> class TypeInfo<int8>
{
public:
   static const bool Integral = true;
   static const bool Unsigned = false;
   static const int  Rank = 0;
   static const int  Width = 1;
   typedef int8 ElementType;
   typedef bool Mask;
};
template <> class TypeInfo<int16>
{
public:
   static const bool Integral = true;
   static const bool Unsigned = false;
   static const int  Rank = 0;
   static const int  Width = 1;
   typedef int16 ElementType;
   typedef bool Mask;
};
template <> class TypeInfo<int32>
{
public:
   static const bool Integral = true;
   static const bool Unsigned = false;
   static const int  Rank = 0;
   static const int  Width = 1;
   typedef int32 ElementType;
   typedef bool Mask;
};

template <> class TypeInfo<int64>
{
public:
   static const bool Integral = true;
   static const bool Unsigned = false;
   static const int  Rank = 0;
   static const int  Width = 1;
   typedef int64 ElementType;
   typedef bool Mask;
};



}; // namespace Burns


#endif // SYSTEM_H





