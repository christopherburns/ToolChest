/* ************************************************************************* *\

                  INTEL CORPORATION PROPRIETARY INFORMATION
     This software is supplied under the terms of a license agreement or
     nondisclosure agreement with Intel Corporation and may not be copied
     or disclosed except in accordance with the terms of that agreement.
     Copyright (C) 2011 Intel Corporation. All Rights Reserved.

     Author: Burns, Christopher A.

\* ************************************************************************* */

#ifndef MASK4_H
#define MASK4_H

#include "SSERegister.h"

namespace Burns
{

template <> class Mask<bool, 4>
{
   ///////////////////////////////////////////////////////////////////////////
   //                      Private Data And Methods                         //
   ///////////////////////////////////////////////////////////////////////////

  /// This shit should be private... why is it not ?
public:
   SSERegister _m;
 public:

   FINLINE Mask(const SSERegister& r) : _m(r) {}

   /// Mask<4> is a base case, no more recursion
   INLINE String internalToString() const
   {
      int mask = bitMask();
      return
         String((mask & 1) ? "1" : "0") + ", " +
               ((mask & 2) ? "1" : "0") + ", " +
               ((mask & 4) ? "1" : "0") + ", " +
               ((mask & 8) ? "1" : "0");
   }

public:

   /// Implementation of Generic Mask<bool, N> interface

   /// Constructors
   FINLINE Mask() {}
   FINLINE Mask(bool t)
      : _m(_mm_cmpeq_epi32(_mm_set_epi32(!t, !t, !t, !t),
      SSERegister::zeroes().m128i())) {}
   FINLINE Mask(const bool t[4])
      : _m(_mm_cmpeq_epi32(_mm_set_epi32(!t[3], !t[2], !t[1], !t[0]),
      SSERegister::zeroes().m128i())) {}
   FINLINE Mask(bool b0, bool b1, bool b2, bool b3)
      : _m(_mm_cmpeq_epi32(_mm_set_epi32(!b3, !b2, !b1, !b0),
      SSERegister::zeroes().m128i())) {}

   /// Does this work??? we'll have to test it
   FINLINE Mask(uint64 bitField) : _m(_mm_set_epi32(!(bitField & 0x1), !((bitField >> 1) & 0x1), !((bitField >> 2) & 0x1), !((bitField >> 3) & 0x1))) {}

   /// Conversion to String
   FINLINE String toString() const
   { return String("[") + internalToString() + "]"; }

   /// Generate a bitmask, the low four bits corrspond to the 4 Mask components
   FINLINE uint32 bitMask() const { return _m.signMask(); }

   /// Access a single element
   FINLINE bool operator [] (Index i) const
   { ASSERT(i >= 0 && i < 4); return bitMask() & (0x1 << i); }


   ////////////////////////////
   // Mathematical Operators //
   ////////////////////////////

   /// Unary not
   FINLINE Mask operator ! () const { return *this ^ Mask(true); }
   FINLINE Mask operator ~ () const { return *this ^ Mask(true); }

   friend FINLINE Mask operator & (const Mask& l, const Mask& r) { return Mask(l._m & r._m); }
   friend FINLINE Mask operator | (const Mask& l, const Mask& r) { return l._m | r._m; }
   friend FINLINE Mask operator ^ (const Mask& l, const Mask& r) { return l._m ^ r._m; }

   FINLINE Mask& operator &= (const Mask& r) { return *this = *this & r; }
   FINLINE Mask& operator |= (const Mask& r) { return *this = *this | r; }
   FINLINE Mask& operator ^= (const Mask& r) { return *this = *this ^ r; }


   ////////////////
   // Comparison //
   ////////////////

   friend FINLINE Mask operator == (const Mask& a, const Mask& b)
   { return SSERegister::i32Equality(a._m, b._m); } //    _mm_cmpeq_epi32(data.m128i(), i.data.m128i()); }
   friend FINLINE Mask operator != (const Mask& a, const Mask& b)
   { return !(a == b); }


   ////////////////
   // Reductions //
   ////////////////

   FINLINE bool reduceAnd() const { return _m.signMask() == 0xf; }
   FINLINE bool reduceOr()  const { return _m.signMask() != 0x0; }
   FINLINE bool all() const       { return reduceAnd(); }
   FINLINE bool none() const      { return _m.signMask() == 0x0; }
   FINLINE bool any() const       { return _m.signMask() != 0x0; }

   FINLINE int  count() const
   {
      static int lookup[] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
      return lookup[bitMask()];
   }

   ////////////////////////////
   // Functional Programming //
   ////////////////////////////

   /// Is this right... ?
   template <typename Functor> FINLINE Mask& map(const Functor& f)
   { return f(*this); }
};

}; // namespace Burns


#endif // MASK4_H