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
/*#if defined(__APPLE__)
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
#elif defined(__GNUG__) || defined(__GNUC__)
   #define ___GNU_COMPILER
#elif defined(__clang__)
   #define ___CLANG
#endif*/

/// Resolve microarchitectural details, such as ISA extentions

/// SIMD ISA extentions and APIs here are mutually exclusive, depend on the
/// available hardware and software environment
/*#if defined(AVX)
   #define ___AVX
#endif

#if defined(SSE)
   #define ___SSE
   #define ___SSE4
#endif

/// 64-bit mode detection (long int, void* are 64 bits)
#if defined(___APPLE)
   #if TARGET_RT_64_BIT
      #define ___X64
   #endif
#endif*/


#if defined(___OSX)

   #include <mach/mach_time.h>
   #include <sys/sysctl.h>
   #include <sys/time.h>
   #include <libkern/OSAtomic.h>

#endif



/////////////////////
// Primitive Types //
/////////////////////

namespace ToolChest
{
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
}


//////////////////////////////
// System Atomic Operations //
//////////////////////////////

namespace ToolChest
{
   
   ///////////////////////////
   // Mising Unix Functions //
   ///////////////////////////

   /// Missing on non-windows platforms
   #if !defined(___WINDOWS_NT)

      inline int64 _abs64(int64 n)           { return ::abs(n); }
      inline int64 _rotl64(int64 a, uint8 b) { return (a << b) | ((uint64)a >> (64-b)); }
      inline int64 _rotr64(int64 a, uint8 b) { return ((uint64)a >> b) | (a << (64-b)); }
      inline int32 _rotl(int32 a, uint8 b)   { return (a << b) | ((uint32)a >> (32-b)); }
      inline int32 _rotr(int32 a, uint8 b)   { return ((uint32)a >> b) | (a << (32-b)); }
      inline int16 _rotl16(int16 a, uint8 b) { return (a << b) | ((uint16)a >> (16-b)); }
      inline int16 _rotr16(int16 a, uint8 b) { return ((uint16)a >> b) | (a << (16-b)); }
      inline int8  _rotl8(int8 a, uint8 b)   { return (a << b) | ((uint8)a >> ( 8-b)); }
      inline int8  _rotr8(int8 a, uint8 b)   { return ((uint8)a >> b) | (a << ( 8-b)); }

      inline void __cpuid(uint32 CPUInfo[], const uint32 InfoType)
      { __asm__ __volatile__("cpuid" : "=a" (CPUInfo[0]), "=b" (CPUInfo[1]), "=c" (CPUInfo[2]), "=d" (CPUInfo[3]) : "a" (InfoType)); }
      inline uint64 __rdtsc(void)
      { uint64 retval; __asm__ __volatile__("rdtsc" : "=A"(retval)); return retval; }

   #endif


   /////////////////////////////
   // Bit Counting Operations //
   /////////////////////////////

   static inline int32 popcnt( const uint8 input ) 
   { 
      const uint8 table[0x100] = 
      {
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
         4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8 
      }; 
      return int32(table[int(input)]);
   }

   static inline int32 popcnt( const uint16 input ) { return popcnt(uint8 (input)) + popcnt(uint8 (input >>  8)); }
   static inline int32 popcnt( const uint32 input ) { return popcnt(uint16(input)) + popcnt(uint16(input >> 16)); }
   static inline int32 popcnt( const uint64 input ) { return popcnt(uint32(input)) + popcnt(uint32(input >> 32)); }

   static inline int8  popcnt( const int8  input ) { return (int8)popcnt(uint8 (input)); }
   static inline int32 popcnt( const int16 input ) { return (int32)popcnt(uint8 (input)) + popcnt(int8 (input >>  8)); }
   static inline int32 popcnt( const int32 input ) { return (int32)popcnt(uint16(input)) + popcnt(int16(input >> 16)); }
   static inline int32 popcnt( const int64 input ) { return (int32)popcnt(uint32(input)) + popcnt(int32(input >> 32)); }




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


   /////////////////////////
   // Miscellaneous Hacks //
   /////////////////////////

   /// Use this in a class definition to prohibit the copy constructor and
   /// standard assignment operators. This is primarily for so-called "Singleton"
   /// classes.
   /// \param c is the name of the class
   #define FORBID_COPY_OR_ASSIGNMENT(c) \
      inline c(const c&) { STATIC_assert(false); } \
      inline c& operator=(const c&) { STATIC_assert(false); return *this; }

   /// Nifty Template-Metaprogramming
   ///
   /// For example, to make sure a templated function is never instantiated with a
   /// Vector3, then call
   ///     TYPE_assertION( !TypesAreEqual<T, Vector3>::value );
   /// at the head of the function body.

   #define TYPE_ASSERTION(a, b) { int dummy[ a , b ? 1 : -1]; }
   template <class A, class B> struct TypesAreEqual { enum { value = false }; };
   template <class A> struct TypesAreEqual<A, A> { enum { value = true }; };

   /// Generalized static assertion. Any constant boolean expression will work.
   template <bool> struct STATIC_ASSERTION_FAILURE;
   template <> struct STATIC_ASSERTION_FAILURE<true> {};
   #define STATIC_assert(f) /*sizeof(STATIC_ASSERTION_FAILURE<(bool)(f)>);*/

   template <typename T, typename U> inline const T& cast(const U& from) { return *(T*)(&from); }
   template <typename T, typename U> inline       T& cast(      U& from) { return *(T*)(&from); }

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

}; // namespace ToolChest


#endif // SYSTEM_H





