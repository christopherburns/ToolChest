#ifndef MASK_H
#define MASK_H

//#include "../std::strings.h"
#include "Mathematics.h"

namespace Mathematics
{

   template <class T, int N> class Vector;

   template <class T, int N> class Mask
   {
      ///////////////////////////////////////////////////////////////////////////
      //                           Public Interface                            //
      ///////////////////////////////////////////////////////////////////////////

   public:

      /// Constructors
      Mask();                     ///< NO initialization
      Mask(const T& t);           ///< Initialize all elements from scalar
      explicit Mask(T t[N]);      ///< Initialize from array

      inline Mask(uint64_t bitField): _a(bitField), _b(bitField >> N/2) {}

      template <class S> explicit Mask(const Mask<S, N>& v);  ///< Conversion constructor

      //inline std::string ToString() const { return std::string("[") + internalToString() + "]"; }

      /// Access a single element
      inline bool operator [] (int i) const
      { assert(i >= 0 && i < N); return (i < WA) ? (_a[i]) : (_b[i-WA]);  }

      // Auto cast to bool...
      inline operator T() { return All(); } 


      ///////////////////////
      // Logical Operators //
      ///////////////////////

      inline Mask operator ! () const                                    { return Mask(!_a, !_b); }
      inline Mask operator ~ () const                                    { return Mask(~_a, ~_b); }

      friend inline Mask operator & (const Mask& l, const Mask& r)       { return Mask(l._a & r._a, l._b & r._b); }
      friend inline Mask operator | (const Mask& l, const Mask& r)       { return Mask(l._a | r._a, l._b | r._b); }
      friend inline Mask operator ^ (const Mask& l, const Mask& r)       { return Mask(l._a ^ r._a, l._b ^ r._b); }

      inline Mask& operator &= (const Mask& r)                           { _a &= r._a; _b &= r._b;  return *this; }
      inline Mask& operator |= (const Mask& r)                           { _a |= r._a; _b |= r._b;  return *this; }
      inline Mask& operator ^= (const Mask& r)                           { _a ^= r._a; _b ^= r._b;  return *this; }


      //////////////////////////
      // Comparison Operators //
      //////////////////////////

      friend inline Mask operator == (const Mask& l, const Mask& r)      { return Mask(l._a == r._a, l._b == r._b); }
      friend inline Mask operator != (const Mask& l, const Mask& r)      { return Mask(l._a != r._a, l._b != r._b); }


      ////////////////
      // Reductions //
      ////////////////

      inline T ReduceAnd() const;
      inline T ReduceOr()  const;
      inline T All() const;
      inline T None() const;
      inline T Any() const;

      int Count() const;   ///< Returns the number of 'true' elements

      ////////////////////////////
      // Functional Programming //
      ////////////////////////////

      template <typename Functor> Mask Map(const Functor& f) const;


      ///////////////////////////////////////////////////////////////////////////
      //                      Private Data And Methods                         //
      ///////////////////////////////////////////////////////////////////////////

   private:
      static const int WA = (1 << ILOG2<N>::value) == N ? N/2 : (1 << ILOG2<N>::value);
      static const int WB = N - WA;

      typedef Mask<T, WA> MaskA;
      typedef Mask<T, WB> MaskB;

     /// Note, this is temporary, until I find out how to make friends
     /// with the update() function (or perhaps static function of Vector<T, N>)
   public:
      MaskA _a;
      MaskB _b;

      /// Private constructor
      inline Mask(const MaskA& a, const MaskB& b) : _a(a), _b(b) {}

      /// Private string I/O, necessary to make use of recursion
      //inline std::string internalToString() const
      //{ return _a.internalToString() + ", " + _b.internalToString(); }
   };


   ///////////////////////////////////////////////////////////////////////////////
   //                       Generic Mask Implementation                         //
   ///////////////////////////////////////////////////////////////////////////////

   template <class T, int N> inline Mask<T, N>::Mask() {}
   template <class T, int N> inline Mask<T, N>::Mask(const T& t) : _a(t), _b(t) {}
   template <class T, int N> inline Mask<T, N>::Mask(T t[N]) : _a(t), _b(t+WA) {}

   template <class T, int N> template <class S>
   inline Mask<T, N>::Mask(const Mask<S, N>& v) : _a(v._a), _b(v._b) {}


   /////////////////////////////////////
   // Reduction Operation Definitions //
   /////////////////////////////////////

   template <class T, int N> inline T Mask<T, N>::ReduceAnd() const
   { return _a.ReduceAnd() & _b.ReduceAnd(); }
   template <class T, int N> inline T Mask<T, N>::ReduceOr()  const
   { return _a.ReduceOr() | _b.ReduceOr(); }

   template <class T, int N> inline T Mask<T, N>::All()  const  { return ReduceAnd(); }
   template <class T, int N> inline T Mask<T, N>::None() const  { return !ReduceOr(); }
   template <class T, int N> inline T Mask<T, N>::Any()  const  { return !None(); }

   template <class T, int N> inline int Mask<T, N>::Count() const
   { return _a.Count() + _b.Count(); }


   ////////////////////////////////////////
   // Functional Programming Definitions //
   ////////////////////////////////////////

   template <class T, int N>
   template <typename Functor> inline Mask<T, N> Mask<T, N>::Map(const Functor& f) const
   { return Mask<T, N>(_a.Map(f), _b.Map(f)); }



   /// Base case implementation, for N == 1
   template <class T> class Mask<T, 1>
   {
      ///////////////////////////////////////////////////////////////////////////
      //                      Private Data And Methods                         //
      ///////////////////////////////////////////////////////////////////////////

   public:
      T _t;

      /// Private string I/O, necessary to make use of recursion
      //inline std::string internalToString() const { return (_t ? "1" : "0"); }

   public:

      /// Implementation of Generic Mask<T, N> interface

      /// Constructors
      inline Mask()                                  {}
      inline Mask(const T& t)             : _t(t)    {}
      inline explicit Mask(const T t[1])  : _t(t[0]) {}
      template <class S> inline explicit Mask(const Mask<S, 1>& v) : _t(v._t) {}

      inline Mask(uint64_t bitMask) : _t(bitMask & 0x1) {}

      inline operator const T() const { return _t; }
      inline operator T&() { return _t; }

      /// Access a single element
      inline bool operator [] (int i) const
      { assert(i == 0); return _t;  }


      /// std::string I/O representation
      //inline std::string ToString() const { return std::string("[") + (_t ? "1" : "0") + "]"; }


      ////////////////////////////
      // Mathematical Operators //
      ////////////////////////////

      /// Unary minus
      inline const Mask operator ! () const { return !_t; }
      inline Mask operator ~ () const { return !_t; }

      friend inline Mask operator & (const Mask& l, const Mask& r)
      { return l._t && r._t; }
      friend inline Mask operator | (const Mask& l, const Mask& r)
      { return l._t || r._t; }
      friend inline Mask operator ^ (const Mask& l, const Mask& r)
      { return l._t ^  r._t; }

      inline Mask& operator &= (const Mask& r) { _t &= r._t; return *this; }
      inline Mask& operator |= (const Mask& r) { _t |= r._t; return *this; }
      inline Mask& operator ^= (const Mask& r) { _t ^= r._t; return *this; }

      ////////////////
      // Comparison //
      ////////////////

      friend inline Mask operator == (const Mask& a, const Mask& b)
      { return a._t == b._t; }
      friend inline Mask operator != (const Mask& a, const Mask& b)
      { return a._t != b._t; }


      ////////////////
      // Reductions //
      ////////////////

      inline T ReduceAnd() const { return _t & _t; }
      inline T ReduceOr()  const { return _t | _t; }

      inline T All()  const  { return ReduceAnd(); }
      inline T None() const  { return !ReduceOr(); }
      inline T Any()  const  { return !None(); }

      inline int Count() const { return (_t ? 1 : 0); }


      ////////////////////////////
      // Functional Programming //
      ////////////////////////////

      template <typename Functor> inline Mask& Map(const Functor& f) { return f(_t); }
   };


   template <class T> bool ANY(const T& a)  { return a.Any(); }
   template <class T> bool NONE(const T& a) { return a.None(); }
   template <class T> bool ALL(const T& a)  { return a.All(); }

}; // namespace Mathematics


#endif // MASK_H