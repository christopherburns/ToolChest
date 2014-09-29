#ifndef SYSTEM_H
#define SYSTEM_H

#include <float.h>
#include <assert.h>
#include <math.h>

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

}; // namespace ToolChest


#endif // SYSTEM_H





