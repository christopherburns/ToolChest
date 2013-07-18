#ifndef MASK_H
#define MASK_H

#include "../Strings.h"
#include "Mathematics.h"

namespace ToolChest
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

   inline Mask(uint64 bitField): _a(bitField), _b(bitField >> N/2) {}

   template <class S> explicit Mask(const Mask<S, N>& v);  ///< Conversion constructor

   inline String toString() const { return String("[") + internalToString() + "]"; }

   /// Access a single element
   inline bool operator [] (int i) const
   { assert(i >= 0 && i < N); return (i < WA) ? (_a[i]) : (_b[i-WA]);  }

   // Auto cast to bool...
   inline operator T() { return all(); } 


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

   inline T reduceAnd() const;
   inline T reduceOr()  const;
   inline T all() const;
   inline T none() const;
   inline T any() const;

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
   inline Mask(const MaskA& a, const MaskB& b) : _a(a), _b(b) {}

   /// Private string I/O, necessary to make use of recursion
   inline String internalToString() const
   { return _a.internalToString() + ", " + _b.internalToString(); }
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

template <class T, int N> inline T Mask<T, N>::reduceAnd() const
{ return _a.reduceAnd() & _b.reduceAnd(); }
template <class T, int N> inline T Mask<T, N>::reduceOr()  const
{ return _a.reduceOr() | _b.reduceOr(); }

template <class T, int N> inline T Mask<T, N>::all()  const  { return reduceAnd(); }
template <class T, int N> inline T Mask<T, N>::none() const  { return !reduceOr(); }
template <class T, int N> inline T Mask<T, N>::any()  const  { return !none(); }

template <class T, int N> inline int Mask<T, N>::count() const
{ return _a.count() + _b.count(); }


////////////////////////////////////////
// Functional Programming Definitions //
////////////////////////////////////////

template <class T, int N>
template <typename Functor> inline Mask<T, N> Mask<T, N>::map(const Functor& f) const
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
   inline String internalToString() const { return (_t ? "1" : "0"); }

public:

   /// Implementation of Generic Mask<T, N> interface

   /// Constructors
   inline Mask()                                  {}
   inline Mask(const T& t)             : _t(t)    {}
   inline explicit Mask(const T t[1])  : _t(t[0]) {}
   template <class S> inline explicit Mask(const Mask<S, 1>& v) : _t(v._t) {}

   inline Mask(uint64 bitMask) : _t(bitMask & 0x1) {}

   inline operator const T() const { return _t; }
   inline operator T&() { return _t; }

   /// Access a single element
   inline bool operator [] (int i) const
   { assert(i == 0); return _t;  }


   /// String I/O representation
   inline String toString() const { return String("[") + (_t ? "1" : "0") + "]"; }


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

   inline T reduceAnd() const { return _t & _t; }
   inline T reduceOr()  const { return _t | _t; }

   inline T all()  const  { return reduceAnd(); }
   inline T none() const  { return !reduceOr(); }
   inline T any()  const  { return !none(); }

   inline int count() const { return (_t ? 1 : 0); }


   ////////////////////////////
   // Functional Programming //
   ////////////////////////////

   template <typename Functor> inline Mask& map(const Functor& f) { return f(_t); }
};


template <class T> bool ANY(const T& a)  { return a.any(); }
template <class T> bool NONE(const T& a) { return a.none(); }
template <class T> bool ALL(const T& a)  { return a.all(); }

}; // namespace ToolChest


#endif // MASK_H