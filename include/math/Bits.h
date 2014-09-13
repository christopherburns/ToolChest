#ifndef BITS_H
#define BITS_H

#include "../System.h"


namespace ToolChest
{

/// An early BitMask class, which we will eventually use to replace the Mask class
/// used for the Vector types... (cannot support multidimensional BitMasks though,
/// and this might be a problem for masking things like Vector3xNf...)
template <int N> class BitMask
{
private:
   uint64 _bits;  /// Generalize this, eventually...

public:
   inline BitMask() { STATIC_assert(N <= 64 && N > 0); }
   inline BitMask(const uint64& b) : _bits(b) { STATIC_assert(N <= 64 && N > 0); }

   /// Accessors and Modifiers
   uint64 ToInteger() const { return _bits; }
   bool operator [] (int i) const { return (bool)((_bits >> i) & 0x1); }
   void Set(int i) { _bits |= (0x1 << i); }
   void Clear(int i) { _bits &= (~0x1 << i); }

   ///////////////////////
   // Logical Operators //
   ///////////////////////

   inline BitMask operator ! () const                                    { return BitMask(~_bits); }
   inline BitMask operator ~ () const                                    { return BitMask(~_bits); }

   friend inline BitMask operator & (const BitMask& l, const BitMask& r) { return BitMask(l._bits & r._bits); }
   friend inline BitMask operator | (const BitMask& l, const BitMask& r) { return BitMask(l._bits | r._bits); }
   friend inline BitMask operator ^ (const BitMask& l, const BitMask& r) { return BitMask(l._bits ^ r._bits); }

   inline BitMask& operator &= (const BitMask& r)                        { _bits &= r._bits; return *this; }
   inline BitMask& operator |= (const BitMask& r)                        { _bits |= r._bits; return *this; }
   inline BitMask& operator ^= (const BitMask& r)                        { _bits ^= r._bits; return *this; }

   //////////////////////////
   // Comparison Operators //
   //////////////////////////

   /// For a bit-by-bit test, use the xor (^) operator
   friend inline bool operator == (const BitMask& l, const BitMask& r)      { return l._bits == r._bits; }
   friend inline bool operator != (const BitMask& l, const BitMask& r)      { return l._bits != r._bits; }


   ////////////////
   // Reductions //
   ////////////////

   //T reduceAnd() const;
   //T reduceOr()  const;
   //T all() const;
   //T none() const;
   //T any() const;

};








///////////////////////////////
// Bit Manipulation Routines //
///////////////////////////////


template <class T> struct BITS
{
   /// Bit rotations
   static inline T Rotr(T a, uint8 b);
   static inline T Rotl(T a, uint8 b);

   /// Bit interleaVes
   static inline T Interleave11(T a, T b);
   static inline T Interleave21(T a, T b);

   /// Bit de-interLeaving
   static inline T Deinterleave11(T bits);

   /// Counts '1' bits in an integer
   static inline T BitCount(T a);

   /// Returns the place of the least significant bit. Results are undefined
   /// for a == ZERO<T>(). msb returns the most significant bit position.
   //static inline T lsb(T a);
   //static inline T msb(T a);
};


////////////////////////////////
// Bit Interleave Definitions //
////////////////////////////////

/// Class providing bit interleave functions for integer types. Not a publicly
/// accessible interface, only for BITS
class BitInterleave
{
private:
   template <class T> static T Interleave11(T a, T b);
   template <class T> static T Interleave21(T a, T B);

   template <class T> static T Deinterleave11(T bits);

   friend struct BITS<uint16>;
   friend struct BITS<uint32>;
   friend struct BITS<uint64>;
};

/// De-interleaves the bits in a 32-bit integer, placing the two results
/// in the high and low 16-bit segments of a 32-bit integer
template <> inline uint32 BitInterleave::Deinterleave11<uint32>(uint32 bits)
{
   uint32 upper = 0, lower = 0;
   for (int i = 0; i < 16; ++i)
   {
      upper |= (bits & 0x1) << i;
      lower |= ((bits & 0x2) >> 1) << i;
      bits = bits >> 2;
   }
   //printf("   upper = 0x%x, lower = 0x%x\n", upper, lower);
   return (upper << 16) | lower;
}

template <> inline uint16 BitInterleave::Deinterleave11<uint16>(uint16 bits)
{
   uint32 upper = 0, lower = 0;
   for (int i = 0; i < 8; ++i)
   {
      upper = (upper << 1) | (bits & 0x1);
      lower = (lower << 1) | (bits & 0x2);
      bits = bits >> 2;
   }
   return uint16((upper << 8) | lower);
}



template <> inline uint16 BitInterleave::Interleave11<uint16>(uint16 b, uint16 a)
{
   uint16 d = 0;
   for (int i = 0; i < 8; i++) {
      uint16 bit_a = (a >> i) & 1;
      uint16 bit_b = (b >> i) & 1;
      d |= (bit_a << (2*i + 0));
      d |= (bit_b << (2*i + 1));
   }
   return d;
}

template <> inline uint32 BitInterleave::Interleave11<uint32>(uint32 b, uint32 a)
{
   uint32 d = 0;
   for (int i=0; i<16; i++) {
      uint32 bit_a = (a >> i) & 1;
      uint32 bit_b = (b >> i) & 1;
      d |= (bit_a << (2*i + 0));
      d |= (bit_b << (2*i + 1));
   }
   return d;
}

template <> inline uint64 BitInterleave::Interleave11<uint64>(uint64 b, uint64 a)
{
   uint64 d = 0;
   for (int i=0; i<32; i++) {
      uint64 bit_a = (a >> i) & 1;
      uint64 bit_b = (b >> i) & 1;
      d |= (bit_a << (2*i + 0));
      d |= (bit_b << (2*i + 1));
   }
   return d;
}

template <> inline uint16 BitInterleave::Interleave21<uint16>(uint16 a, uint16 b)
{
   uint16 d = 0;
   for (short i=0; i<16/3; i++) {
      uint16 bit_a = (a >> 2*i) & 3;
      uint16 bit_b = (b >> i) & 1;
      d |= (bit_a << (3*i + 1));
      d |= (bit_b << (3*i + 0));
   }
   return d;
}

template <> inline uint32 BitInterleave::Interleave21<uint32>(uint32 a, uint32 b)
{
   // from the back: 1 bit of b, then 2 bits of a
   uint32 d = 0;
   for (int i=0; i<32/3; i++) {
      uint32 bit_a = (a >> 2*i) & 3;
      uint32 bit_b = (b >> i) & 1;
      d |= (bit_a << (3*i + 1));
      d |= (bit_b << (3*i + 0));
   }
   return d;
}

template <> inline uint64 BitInterleave::Interleave21<uint64>(uint64 b, uint64 a)
{
   uint64 d = 0;
   for (long i=0; i<64/3; i++) {
      uint64 bit_a = (a >> 2*i) & 3;
      uint64 bit_b = (b >> i) & 1;
      d |= (bit_a << (3*i + 1));
      d |= (bit_b << (3*i + 0));
   }
   return d;
}



/// Specializations
///
/// Many of the signed integer specializations pass the buck to the unsigned
/// versions

template <> struct BITS<uint8>
{
   /// Bit rotations
   static inline uint8 Rotr(uint8 a, uint8 b)           { return _rotr8(a, b); }
   static inline uint8 Rotl(uint8 a, uint8 b)           { return _rotl8(a, b); }
};

template <> struct BITS<uint16>
{
   /// Bit rotations
   static inline uint16 Rotr(uint16 a, uint8 b)           { return _rotr16(a, b); }
   static inline uint16 Rotl(uint16 a, uint8 b)           { return _rotl16(a, b); }

   /// Bit population count
   static inline uint16 BitCount(uint16 b)                { return (uint16)popcnt(b); }

   /// Bit Interleaves
   static inline uint16 Interleave11(uint16 a, uint16 b)
   { return BitInterleave::Interleave11<uint16>(a, b); }
   static inline uint16 Interleave21(uint16 a, uint16 b)
   { return BitInterleave::Interleave21<uint16>(a, b); }

   static inline uint16 Deinterleave11(uint16 bits)
   { return BitInterleave::Deinterleave11<uint16>(bits); }
};

template <> struct BITS<uint32>
{
   /// Bit rotations
   static inline uint32 Rotr(uint32 a, uint8 b)           { return _rotr((int32)a, b); }
   static inline uint32 Rotl(uint32 a, uint8 b)           { return _rotl((int32)a, b); }

   /// Bit population count
   static inline uint32 BitCount(uint32 b)                { return popcnt(b); }

   /// Bit Interleaves
   static inline uint32 Interleave11(uint32 a, uint32 b)
   { return BitInterleave::Interleave11<uint32>(a, b); }
   static inline uint32 Interleave21(uint32 a, uint32 b)
   { return BitInterleave::Interleave21<uint32>(a, b); }

   static inline uint32 Deinterleave11(uint32 bits)
   { return BitInterleave::Deinterleave11<uint32>(bits); }
};

template <> struct BITS<uint64>
{
   /// Bit rotations
   static inline uint64 Rotr(uint64 a, uint8 b)           { return _rotr64((int64)a, b); }
   static inline uint64 Rotl(uint64 a, uint8 b)           { return _rotl64((int64)a, b); }

   /// Bit population count
   static inline int64 BitCount(int64 b)                { return popcnt(b); }

   /// Bit Interleaves
   static inline uint64 Interleave11(uint64 a, uint64 b)
   { return BitInterleave::Interleave11<uint64>(a, b); }
   static inline uint64 Interleave21(uint64 a, uint64 b)
   { return BitInterleave::Interleave21<uint64>(a, b); }
};


template <> struct BITS<int8>
{
   /// Bit rotations
   static inline int8 Rotr(int8 a, uint8 b)           { return _rotr8(a, b); }
   static inline int8 Rotl(int8 a, uint8 b)           { return _rotl8(a, b); }
};

template <> struct BITS<int16>
{
   /// Bit rotations
   static inline int16 Rotr(int16 a, uint8 b)           { return _rotr16(a, b); }
   static inline int16 Rotl(int16 a, uint8 b)           { return _rotl16(a, b); }

   /// Bit population count
   static inline int16 BitCount(int16 b)                { return (int16)popcnt(b); }

   /// Bit Interleaves
   static inline int16 Interleave11(int16 a, int16 b)
   { return (int16)BITS<uint16>::Interleave11((uint16)a, (uint16)b); }
   static inline int16 Interleave21(int16 a, int16 b)
   { return (int16)BITS<uint16>::Interleave21((uint16)a, (uint16)b); }
};

template <> struct BITS<int32>
{
   /// Bit rotations
   static inline int32 Rotr(int32 a, uint8 b)           { return _rotr(a, b); }
   static inline int32 Rotl(int32 a, uint8 b)           { return _rotl(a, b); }

   /// Bit population count
   static inline int32 BitCount(int32 b)                { return popcnt(b); }

   /// Bit Interleaves
   static inline int32 Interleave11(int32 a, int32 b)
   { return (int32)BITS<uint32>::Interleave11((uint32)a, (uint32)b); }
   static inline int32 Interleave21(int32 a, int32 b)
   { return (int32)BITS<uint32>::Interleave21((uint32)a, (uint32)b); }
};

template <> struct BITS<int64>
{
   /// Bit rotations
   static inline int64 Rotr(int64 a, uint8 b)           { return _rotr64(a, b); }
   static inline int64 Rotl(int64 a, uint8 b)           { return _rotl64(a, b); }

   /// Bit population count
   static inline int64 BitCount(int64 b)                { return popcnt(b); }

   /// Bit Interleaves
   static inline int64 Interleave11(int64 a, int64 b)
   { return (int64)BITS<uint64>::Interleave11((uint64)a, (uint64)b); }
   static inline int64 Interleave21(int64 a, int64 b)
   { return (int64)BITS<uint64>::Interleave21((uint64)a, (uint64)b); }
};




/// Global interface for BITS namespace functions
template <class T> inline T ROTR(T a, uint8 b)     { return BITS<T>::Rotr(a, b); }
template <class T> inline T ROTL(T a, uint8 b)     { return BITS<T>::Rotl(a, b); }
template <class T> inline T INTERLEAVE11(T a, T b) { return BITS<T>::interleave11(a, b); }
template <class T> inline T INTERLEAVE21(T a, T b) { return BITS<T>::interleave21(a, b); }
template <class T> inline T DEINTERLEAVE11(T bits) { return BITS<T>::deinterleave11(bits); }
template <class T> inline T BITCOUNT(T b)          { return BITS<T>::BitCount(b); }

}; // namespace ToolChest


#endif // BITS_H