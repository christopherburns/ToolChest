#ifndef MASK_H
#define MASK_H

#include "../Strings.h"
#include "Mathematics.h"

namespace Burns
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

   FINLINE Mask(uint64 bitField): _a(bitField), _b(bitField >> N/2) {}

   template <class S> explicit Mask(const Mask<S, N>& v);  ///< Conversion constructor

   FINLINE String toString() const { return String("[") + internalToString() + "]"; }

   /// Access a single element
   FINLINE bool operator [] (Index i) const
   { ASSERT(i >= 0 && i < N); return (i < WA) ? (_a[i]) : (_b[i-WA]);  }

   // Auto cast to bool...
   FINLINE operator T() { return all(); } 


   ///////////////////////
   // Logical Operators //
   ///////////////////////

   FINLINE Mask operator ! () const                                    { return Mask(!_a, !_b); }
   FINLINE Mask operator ~ () const                                    { return Mask(~_a, ~_b); }

   friend FINLINE Mask operator & (const Mask& l, const Mask& r)       { return Mask(l._a & r._a, l._b & r._b); }
   friend FINLINE Mask operator | (const Mask& l, const Mask& r)       { return Mask(l._a | r._a, l._b | r._b); }
   friend FINLINE Mask operator ^ (const Mask& l, const Mask& r)       { return Mask(l._a ^ r._a, l._b ^ r._b); }

   FINLINE Mask& operator &= (const Mask& r)                           { _a &= r._a; _b &= r._b;  return *this; }
   FINLINE Mask& operator |= (const Mask& r)                           { _a |= r._a; _b |= r._b;  return *this; }
   FINLINE Mask& operator ^= (const Mask& r)                           { _a ^= r._a; _b ^= r._b;  return *this; }


   //////////////////////////
   // Comparison Operators //
   //////////////////////////

   friend FINLINE Mask operator == (const Mask& l, const Mask& r)      { return Mask(l._a == r._a, l._b == r._b); }
   friend FINLINE Mask operator != (const Mask& l, const Mask& r)      { return Mask(l._a != r._a, l._b != r._b); }


   ////////////////
   // Reductions //
   ////////////////

   FINLINE T reduceAnd() const;
   FINLINE T reduceOr()  const;
   FINLINE T all() const;
   FINLINE T none() const;
   FINLINE T any() const;

   int count() const;   ///< Returns the number of 'true' elements

   ////////////////////////////
   // Functional Programming //
   ////////////////////////////

   template <typename Functor> Mask map(const Functor& f) const;


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
   FINLINE Mask(const MaskA& a, const MaskB& b) : _a(a), _b(b) {}

   /// Private string I/O, necessary to make use of recursion
   FINLINE String internalToString() const
   { return _a.internalToString() + ", " + _b.internalToString(); }
};


///////////////////////////////////////////////////////////////////////////////
//                       Generic Mask Implementation                         //
///////////////////////////////////////////////////////////////////////////////

template <class T, int N> FINLINE Mask<T, N>::Mask() {}
template <class T, int N> FINLINE Mask<T, N>::Mask(const T& t) : _a(t), _b(t) {}
template <class T, int N> FINLINE Mask<T, N>::Mask(T t[N]) : _a(t), _b(t+WA) {}

template <class T, int N> template <class S>
FINLINE Mask<T, N>::Mask(const Mask<S, N>& v) : _a(v._a), _b(v._b) {}


/////////////////////////////////////
// Reduction Operation Definitions //
/////////////////////////////////////

template <class T, int N> FINLINE T Mask<T, N>::reduceAnd() const
{ return _a.reduceAnd() & _b.reduceAnd(); }
template <class T, int N> FINLINE T Mask<T, N>::reduceOr()  const
{ return _a.reduceOr() | _b.reduceOr(); }

template <class T, int N> FINLINE T Mask<T, N>::all()  const  { return reduceAnd(); }
template <class T, int N> FINLINE T Mask<T, N>::none() const  { return !reduceOr(); }
template <class T, int N> FINLINE T Mask<T, N>::any()  const  { return !none(); }

template <class T, int N> FINLINE int Mask<T, N>::count() const
{ return _a.count() + _b.count(); }


////////////////////////////////////////
// Functional Programming Definitions //
////////////////////////////////////////

template <class T, int N>
template <typename Functor> FINLINE Mask<T, N> Mask<T, N>::map(const Functor& f) const
{ return Mask<T, N>(_a.map(f), _b.map(f)); }



/// Base case implementation, for N == 1
template <class T> class Mask<T, 1>
{
   ///////////////////////////////////////////////////////////////////////////
   //                      Private Data And Methods                         //
   ///////////////////////////////////////////////////////////////////////////

public:
   T _t;

   /// Private string I/O, necessary to make use of recursion
   FINLINE String internalToString() const { return (_t ? "1" : "0"); }

public:

   /// Implementation of Generic Mask<T, N> interface

   /// Constructors
   FINLINE Mask()                                  {}
   FINLINE Mask(const T& t)             : _t(t)    {}
   FINLINE explicit Mask(const T t[1])  : _t(t[0]) {}
   template <class S> FINLINE explicit Mask(const Mask<S, 1>& v) : _t(v._t) {}

   FINLINE Mask(uint64 bitMask) : _t(bitMask & 0x1) {}

   FINLINE operator const T() const { return _t; }
   FINLINE operator T&() { return _t; }

   /// Access a single element
   FINLINE bool operator [] (Index i) const
   { ASSERT(i == 0); return _t;  }


   /// String I/O representation
   FINLINE String toString() const { return String("[") + (_t ? "1" : "0") + "]"; }


   ////////////////////////////
   // Mathematical Operators //
   ////////////////////////////

   /// Unary minus
   FINLINE const Mask operator ! () const { return !_t; }
   FINLINE Mask operator ~ () const { return !_t; }

   friend FINLINE Mask operator & (const Mask& l, const Mask& r)
   { return l._t && r._t; }
   friend FINLINE Mask operator | (const Mask& l, const Mask& r)
   { return l._t || r._t; }
   friend FINLINE Mask operator ^ (const Mask& l, const Mask& r)
   { return l._t ^  r._t; }

   FINLINE Mask& operator &= (const Mask& r) { _t &= r._t; return *this; }
   FINLINE Mask& operator |= (const Mask& r) { _t |= r._t; return *this; }
   FINLINE Mask& operator ^= (const Mask& r) { _t ^= r._t; return *this; }

   ////////////////
   // Comparison //
   ////////////////

   friend FINLINE Mask operator == (const Mask& a, const Mask& b)
   { return a._t == b._t; }
   friend FINLINE Mask operator != (const Mask& a, const Mask& b)
   { return a._t != b._t; }


   ////////////////
   // Reductions //
   ////////////////

   FINLINE T reduceAnd() const { return _t & _t; }
   FINLINE T reduceOr()  const { return _t | _t; }

   FINLINE T all()  const  { return reduceAnd(); }
   FINLINE T none() const  { return !reduceOr(); }
   FINLINE T any()  const  { return !none(); }

   FINLINE int count() const { return (_t ? 1 : 0); }


   ////////////////////////////
   // Functional Programming //
   ////////////////////////////

   template <typename Functor> FINLINE Mask& map(const Functor& f) { return f(_t); }
};


template <class T> bool ANY(const T& a)  { return a.any(); }
template <class T> bool NONE(const T& a) { return a.none(); }
template <class T> bool ALL(const T& a)  { return a.all(); }

}; // namespace Burns


#endif // MASK_H