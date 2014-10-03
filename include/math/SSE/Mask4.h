#ifndef MASK4_H
#define MASK4_H

#include "SSERegister.h"

namespace Mathematics
{

   template <> class Mask<bool, 4>
   {
      ///////////////////////////////////////////////////////////////////////////
      //                      Private Data And Methods                         //
      ///////////////////////////////////////////////////////////////////////////

      /// This shit should be private... why is it not ?
      SSERegister _m;

      friend class Vector<int32_t, 4>;
      friend class Vector<float  , 4>;

   private:

      inline Mask(const SSERegister& r) : _m(r) {}

      /// Mask<4> is a base case, no more recursion
      inline std::string internalToString() const
      {
         int mask = BitMask();
         return
            std::string((mask & 1) ? "1" : "0") + ", " +
                  ((mask & 2) ? "1" : "0") + ", " +
                  ((mask & 4) ? "1" : "0") + ", " +
                  ((mask & 8) ? "1" : "0");
      }

   public:

      /// Implementation of Generic Mask<bool, N> interface

      /// Constructors
      inline Mask() {}
      inline Mask(bool t)
         : _m(_mm_cmpeq_epi32(_mm_set_epi32(!t, !t, !t, !t),
         SSERegister::zeroes().m128i())) {}
      inline Mask(const bool t[4])
         : _m(_mm_cmpeq_epi32(_mm_set_epi32(!t[3], !t[2], !t[1], !t[0]),
         SSERegister::zeroes().m128i())) {}
      inline Mask(bool b0, bool b1, bool b2, bool b3)
         : _m(_mm_cmpeq_epi32(_mm_set_epi32(!b3, !b2, !b1, !b0),
         SSERegister::zeroes().m128i())) {}

      /// Does this work??? we'll have to test it
      inline Mask(uint64_t bitField) : _m(_mm_set_epi32(!(bitField & 0x1), !((bitField >> 1) & 0x1), !((bitField >> 2) & 0x1), !((bitField >> 3) & 0x1))) {}

      /// Conversion to std::string
      inline std::string ToString() const
      { return std::string("[") + internalToString() + "]"; }

      /// Generate a bitmask, the low four bits corrspond to the 4 Mask components
      inline uint32_t BitMask() const { return _m.signMask(); }

      /// Access a single element
      inline bool operator [] (int i) const
      { assert(i >= 0 && i < 4); return BitMask() & (0x1 << i); }


      ////////////////////////////
      // Mathematical Operators //
      ////////////////////////////

      /// Unary not
      inline Mask operator ! () const { return *this ^ Mask(true); }
      inline Mask operator ~ () const { return *this ^ Mask(true); }

      friend inline Mask operator & (const Mask& l, const Mask& r) { return Mask(l._m & r._m); }
      friend inline Mask operator | (const Mask& l, const Mask& r) { return l._m | r._m; }
      friend inline Mask operator ^ (const Mask& l, const Mask& r) { return l._m ^ r._m; }

      inline Mask& operator &= (const Mask& r) { return *this = *this & r; }
      inline Mask& operator |= (const Mask& r) { return *this = *this | r; }
      inline Mask& operator ^= (const Mask& r) { return *this = *this ^ r; }


      ////////////////
      // Comparison //
      ////////////////

      friend inline Mask operator == (const Mask& a, const Mask& b)
      { return SSERegister::i32Equality(a._m, b._m); } //    _mm_cmpeq_epi32(data.m128i(), i.data.m128i()); }
      friend inline Mask operator != (const Mask& a, const Mask& b)
      { return !(a == b); }


      ////////////////
      // Reductions //
      ////////////////

      inline bool ReduceAnd() const { return _m.signMask() == 0xf; }
      inline bool ReduceOr()  const { return _m.signMask() != 0x0; }
      inline bool All() const       { return ReduceAnd(); }
      inline bool None() const      { return _m.signMask() == 0x0; }
      inline bool Any() const       { return _m.signMask() != 0x0; }

      inline int  Count() const
      {
         static int lookup[] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
         return lookup[BitMask()];
      }

      ////////////////////////////
      // Functional Programming //
      ////////////////////////////

      /// Is this right... ?
      template <typename Functor> inline Mask& Map(const Functor& f)
      { return f(*this); }
   };

}; // namespace Mathematics


#endif // MASK4_H